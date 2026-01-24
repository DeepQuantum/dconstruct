#include <gtest/gtest.h>
#include "binaryfile.h"
#include "decompilation/decomp_function.h"
#include <fstream>
#include "compilation/function.h"
#include "disassembly/file_disassembler.h"

namespace dconstruct::testing {

    static SIDBase base = *SIDBase::from_binary(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\test_sidbase.bin)");


    TEST(BINARYFILE, Read1) {
        std::vector<Instruction> istrs = {
            {Opcode::LoadStaticPointerImm, 0, 0, 0},
            {Opcode::Return, 0, 0, 0},
        };

        std::vector<u64> symbol_table = {0x0};
        

        std::vector<compiler::function> funcs(2);
        for (auto& fn : funcs) {
            fn.m_instructions = istrs;
            fn.m_symbolTable = symbol_table;
            fn.m_symbolTableEntryPointers.push_back(compiler::function::SYMBOL_TABLE_POINTER_KIND::STRING);
        }
        compiler::global_state gs;
        gs.m_strings.push_back("test");
        gs.m_strings.push_back("testasdasd");

        auto b = *BinaryFile<>::from_codegen(funcs, gs);
        FileDisassembler<true> disassembler(&b, &base, "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/stuff.bin", {});
        disassembler.disassemble();
        disassembler.dump();
    }

    TEST(BINARYFILE, Transplant1) {
        BinaryFile<> file = *BinaryFile<>::from_path("C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/ss-rogue/rogue-encounter-defines.bin");
        FileDisassembler<true> disassembler(&file, &base, "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/transplant_check.bin", {});
        disassembler.disassemble();

        std::vector<compiler::function> converted;
        compiler::global_state global;

        for (const auto& f : disassembler.get_functions()) {
            compiler::function cf;
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
                const compiler::function::SYMBOL_TABLE_POINTER_KIND kind = std::visit([](auto&& type) {
                    using T = std::decay_t<decltype(type)>;
                    if constexpr (std::is_same_v<T, ast::primitive_type>) {
                        return type.m_type == ast::primitive_kind::STRING ? compiler::function::SYMBOL_TABLE_POINTER_KIND::STRING : compiler::function::SYMBOL_TABLE_POINTER_KIND::NONE;
                    } else if constexpr (std::is_same_v<T, ast::ptr_type>) {
                        return compiler::function::SYMBOL_TABLE_POINTER_KIND::GENERAL;
                    } else {
                        return compiler::function::SYMBOL_TABLE_POINTER_KIND::NONE;
                    }
                }, f.m_stackFrame.m_symbolTable.m_types[i]);
                if (kind == compiler::function::SYMBOL_TABLE_POINTER_KIND::STRING) {
                    const u32 size = global.add_string(f.m_stackFrame.m_symbolTable.m_location.get<const char*>(i * 8));
                    cf.m_symbolTable.push_back(size);
                } else {
                    cf.m_symbolTable.push_back(f.m_stackFrame.m_symbolTable.m_location.get<u64>(i * 8));
                }
                cf.m_symbolTableEntryPointers.push_back(kind);
            }
            converted.push_back(std::move(cf));
        }
        auto b = *BinaryFile<>::from_codegen(converted, global);
        FileDisassembler<true> new_disassembler(& b, &base, "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/transplant_check.bin", {});
        new_disassembler.disassemble();
        new_disassembler.dump();
        
    }


}