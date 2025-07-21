#include "decompiler.h"
#include "binaryfile.h"
#include "file_disassembler.h"
#include <gtest/gtest.h>
#include <filesystem>

#define GET_FUNC(p, o) 

namespace dconstruct::testing {

    static function_disassembly get_function_disassembly(const std::string &path, const u32 offset) {
        SIDBase base{}; 
        base.load("dc_test_files/test_sidbase.bin"); 
        BinaryFile file(path); 
        FileDisassembler disassembler(&file, &base, "", DisassemblerOptions{}); 
        const ScriptLambda *lambda_ptr = disassembler.get_value_ptr_at<ScriptLambda>(offset); 
        const function_disassembly fd = disassembler.create_function_disassembly(lambda_ptr);
    }

    TEST(DECOMPILER, BasicAdd) {
        const function_disassembly fd = get_function_disassembly("dc_test_files/ss-fx-funcs.bin", 0x60);
        ASSERT_EQ(fd.m_lines[0].m_instruction.opcode, Opcode::Move);
    }

}
