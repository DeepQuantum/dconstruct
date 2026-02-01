#include <gtest/gtest.h>
#include "binaryfile.h"
#include "decompilation/decomp_function.h"
#include <fstream>
#include "compilation/function.h"
#include "disassembly/file_disassembler.h"

namespace dconstruct::testing {

    static SIDBase base = *SIDBase::from_binary(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\test_sidbase.bin)");

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
        FileDisassembler disassembler(&file, &base, "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/transplant_check.bin", {});
        disassembler.disassemble();

        std::vector<compilation::function> converted;
        compilation::global_state global;

        std::vector<function_disassembly> funcs = disassembler.get_functions();
        std::ranges::sort(funcs, [](const function_disassembly& a, const function_disassembly& b) {
            return a.m_originalOffset < b.m_originalOffset;
        });

        for (const auto& f : funcs) {
            compilation::function cf;
            const std::string id = f.get_id();
            if (id.starts_with("#")) {
                cf.m_name = std::stoull(id.substr(1, id.size() - 1), nullptr, 16);
            } else {
                cf.m_name = id;
            }
            for (const auto& line : f.m_lines) {
                cf.m_instructions.push_back(line.m_instruction);
            }
            for (u32 i = 0; i < f.m_stackFrame.m_symbolTable.m_types.size(); ++i) {
                const compilation::function::SYMBOL_TABLE_POINTER_KIND kind = std::visit([](auto&& type) {
                    using T = std::decay_t<decltype(type)>;
                    if constexpr (std::is_same_v<T, ast::primitive_type>) {
                        return type.m_type == ast::primitive_kind::STRING ? compilation::function::SYMBOL_TABLE_POINTER_KIND::STRING : compilation::function::SYMBOL_TABLE_POINTER_KIND::NONE;
                    } else if constexpr (std::is_same_v<T, ast::ptr_type>) {
                        return compilation::function::SYMBOL_TABLE_POINTER_KIND::GENERAL;
                    } else {
                        return compilation::function::SYMBOL_TABLE_POINTER_KIND::NONE;
                    }
                }, f.m_stackFrame.m_symbolTable.m_types[i]);
                if (kind == compilation::function::SYMBOL_TABLE_POINTER_KIND::STRING) {
                    const u32 size = global.add_string(f.m_stackFrame.m_symbolTable.m_location.get<const char*>(i * 8));
                    cf.m_symbolTable.push_back(size);
                } else {
                    cf.m_symbolTable.push_back(f.m_stackFrame.m_symbolTable.m_location.get<u64>(i * 8));
                }
                cf.m_symbolTableEntryPointers.push_back(kind);
            }
            converted.push_back(std::move(cf));
        }
    
        const auto compiled = ast::program::make_binary(converted, global);
        if (compiled) {
            std::ofstream out(result, std::ios::binary);
            out.write(reinterpret_cast<const char*>(compiled->first.get()), compiled->second);
            out.flush();
        }
    }


}