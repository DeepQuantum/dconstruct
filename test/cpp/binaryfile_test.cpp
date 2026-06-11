#include <gtest/gtest.h>
#include "binaryfile.h"
#include "decompilation/decomp_function.h"
#include <fstream>
#include "compilation/function.h"
#include "disassembly/disassembler.h"
#include "disassembly/edit_disassembler.h"

namespace dconstruct::testing {

    static const std::filesystem::path TEST_ROOT = DCONSTRUCT_TEST_ROOT;
    static SIDBase base = *SIDBase::from_binary(TEST_ROOT / "fixtures" / "dc" / "test_sidbase.bin");

    TEST(BINARYFILE, ExpandStringTable) {
        BinaryFile file = *BinaryFile::from_path(TEST_ROOT / "fixtures" / "dc" / "accessibility-script-funcs.bin");

        Disassembler before_disassembler(&file, &base);
        const std::string before = before_disassembler.disassembly_to_string(before_disassembler.disassemble());

        const u32 old_text_size = file.m_dcheader->m_textSize;
        const u32 old_reloc_table_size = *reinterpret_cast<const u32*>(file.m_bytes.get() + old_text_size);
        const u32 old_num_entries = file.m_dcheader->m_numEntries;

        EditDisassembler editor(&file, &base, {});
        const std::string new_string = "dconstruct-expand-test";
        editor.expand_binary_file_string_table(file, new_string);

        EXPECT_EQ(file.m_dcheader->m_magic, DC_MAGIC);
        EXPECT_EQ(file.m_dcheader->m_versionNumber, DC_VERSION);
        EXPECT_EQ(file.m_dcheader->m_numEntries, old_num_entries);
        EXPECT_EQ(file.m_dcheader->m_textSize, old_text_size + new_string.size() + 1);
        EXPECT_STREQ(reinterpret_cast<const char*>(file.m_bytes.get() + old_text_size), new_string.c_str());

        const u32 new_reloc_table_size = *reinterpret_cast<const u32*>(file.m_bytes.get() + file.m_dcheader->m_textSize);
        EXPECT_GE(new_reloc_table_size, old_reloc_table_size);
        EXPECT_GE(static_cast<u64>(new_reloc_table_size) * 64, static_cast<u64>(file.m_dcheader->m_textSize));
        EXPECT_EQ(file.m_size, file.m_dcheader->m_textSize + sizeof(u32) + new_reloc_table_size);

        Disassembler after_disassembler(&file, &base);
        const std::string after = after_disassembler.disassembly_to_string(after_disassembler.disassemble());
        EXPECT_EQ(before, after);

        const std::filesystem::path tmp = std::filesystem::temp_directory_path() / "dconstruct_expand_test.bin";
        {
            const auto unmapped = file.get_unmapped();
            std::ofstream out(tmp, std::ios::binary);
            out.write(reinterpret_cast<const char*>(unmapped.get()), file.m_size);
        }
        auto reloaded = BinaryFile::from_path(tmp);
        ASSERT_TRUE(reloaded.has_value());
        EXPECT_EQ(reloaded->m_size, file.m_size);
        EXPECT_STREQ(reinterpret_cast<const char*>(reloaded->m_bytes.get() + old_text_size), new_string.c_str());

        Disassembler reloaded_disassembler(&*reloaded, &base);
        const std::string reloaded_text = reloaded_disassembler.disassembly_to_string(reloaded_disassembler.disassemble());
        EXPECT_EQ(before, reloaded_text);

        std::filesystem::remove(tmp);
    }

    TEST(BINARYFILE, Transplant1) {
        std::filesystem::path input = "C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/rogue/script-callbacks.bin";

        std::filesystem::path result = "C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/mods/test/bin/dc1/";
        bool take = false;

        for (const auto& part : input) {
            if (take) {
                result /= part;
            }
            if (part == "dc1") {
                take = true;
            }
        }
        std::filesystem::create_directories(result.parent_path());

        BinaryFile file = *BinaryFile::from_path(input);
        Disassembler disassembler(&file, &base);
        disassembler.disassemble();

        std::vector<compilation::function> converted;
        compilation::global_state global;

        auto funcs = disassembler.get_all_functions();
        std::ranges::sort(
            funcs,
            [](const function_disassembly* a, const function_disassembly* b) {
                return a->m_originalOffset < b->m_originalOffset;
            }
        );

        for (const auto* f : funcs) {
            compilation::function cf;
            const std::string id = f->get_id();
            if (id.starts_with("#")) {
                cf.m_name = std::stoull(id.substr(1, id.size() - 1), nullptr, 16);
            } else {
                cf.m_name = id;
            }
            for (const auto& line : f->m_lines) {
                cf.m_instructions.push_back(line.m_instruction);
            }
            for (u32 i = 0; i < f->m_stackFrame.m_symbolTable.m_types.size(); ++i) {
                const compilation::function::SYMBOL_TABLE_POINTER_KIND kind = std::visit(
                    [](auto&& type) {
                        using T = std::decay_t<decltype(type)>;
                        if constexpr (std::is_same_v<T, ast::primitive_type>) {
                            return type.m_type == ast::primitive_kind::STRING ? compilation::function::SYMBOL_TABLE_POINTER_KIND::STRING : compilation::function::SYMBOL_TABLE_POINTER_KIND::NONE;
                        } else if constexpr (std::is_same_v<T, ast::ptr_type>) {
                            return compilation::function::SYMBOL_TABLE_POINTER_KIND::GENERAL;
                        } else {
                            return compilation::function::SYMBOL_TABLE_POINTER_KIND::NONE;
                        }
                    },
                    f->m_stackFrame.m_symbolTable.m_types[i]
                );
                if (kind == compilation::function::SYMBOL_TABLE_POINTER_KIND::STRING) {
                    const u32 size = global.add_string(f->m_stackFrame.m_symbolTable.m_location.get<const char*>(i * 8));
                    cf.m_symbolTable.push_back(size);
                } else {
                    cf.m_symbolTable.push_back(f->m_stackFrame.m_symbolTable.m_location.get<u64>(i * 8));
                }
                cf.m_symbolTableEntryPointers.push_back(kind);
            }
            converted.push_back(std::move(cf));
        }

        std::vector<compilation::program_binary_element> binary_elements;
        binary_elements.reserve(converted.size());
        for (const auto& function : converted) {
            binary_elements.push_back(function.to_binary_element());
        }

        const auto compiled = ast::program::make_binary(std::move(binary_elements), global);
        if (compiled) {
            std::ofstream out(result, std::ios::binary);
            out.write(reinterpret_cast<const char*>(compiled->first.get()), compiled->second);
            out.flush();
        }
    }

}
