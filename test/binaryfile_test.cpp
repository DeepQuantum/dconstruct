#include <gtest/gtest.h>
#include "binaryfile.h"
#include "disassembly/file_disassembler.h"
#include "decompilation/decomp_function.h"
#include <fstream>
#include "compilation/function.h"

namespace dconstruct::testing {

    static SIDBase base = *SIDBase::from_binary(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\uc4\sidbase_sorted.bin)");


    TEST(BINARYFILE, Read1) {
        std::vector<Instruction> istrs = {
            {Opcode::Move, 0, 0, 0},
            {Opcode::Return, 0, 0, 0},
        };

        std::vector<Instruction> istrs2 = {
            {Opcode::Move, 0, 0, 0},
            {Opcode::Move, 1, 0, 0},
            {Opcode::Return, 0, 0, 0},
        };

        std::vector<u64> symbol_table1 = {0xAABBCCDDEE};
        std::vector<u64> symbol_table2 = {0xAABBCCDDEE};
        

        std::vector<compiler::function> funcs(2);
        funcs[0].m_instructions = std::move(istrs);
        funcs[0].m_symbolTable = std::move(symbol_table1);

        funcs[1].m_instructions = std::move(istrs2);
        funcs[1].m_symbolTable = std::move(symbol_table2);


        compiler::global_state gs;
        gs.m_strings.push_back("test");
        gs.m_strings.push_back("testasdasd");

        BinaryFile<>::from_codegen(funcs, gs);
    }


}