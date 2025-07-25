#include "decompiler.h"
#include "binaryfile.h"
#include "file_disassembler.h"
#include "binary_expressions.h"
#include "literal.h"
#include <array>
#include <gtest/gtest.h>
#include <filesystem>

const std::string TEST_DIR = "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/dc_test_files/";

namespace dconstruct::testing {

    static function_disassembly get_function_disassembly(const std::string &path, const u32 offset) {
        SIDBase base{}; 
        base.load(TEST_DIR + "test_sidbase.bin"); 
        BinaryFile file(TEST_DIR + path); 
        file.dc_setup();
        FileDisassembler disassembler(&file, &base, "", DisassemblerOptions{}); 
        const ScriptLambda *lambda_ptr = disassembler.get_value_ptr_at<ScriptLambda>(offset); 
        const function_disassembly fd = disassembler.create_function_disassembly(lambda_ptr);
        return fd;
    }

    template<std::size_t array_size>
    dcompiler::expression_frame make_expression_frame(std::array<Instruction, array_size> istrs) {
        const StackFrame sf{};
        std::vector<function_disassembly_line> lines{};
        for (u32 i = 0; i < array_size; ++i) {
            lines.emplace_back(i, &istrs[i]);
        }
        function_disassembly fd{std::move(lines), std::move(sf), "Test"};
        dcompiler::Decompiler dc(&fd);
        std::vector<dcompiler::decompiled_function> funcs = dc.decompile();
        return std::move(funcs[0].m_frame);
    }

    TEST(DECOMPILER, BasicAdd) {
        const std::array<Instruction, 1> istrs = {
            {Opcode::LoadU16Imm, 0, 1, 0},
        };
        const dcompiler::expression_frame frame = make_expression_frame(istrs);

        const auto& actual = *dynamic_cast<const ast::assign_expr*>(frame.m_expressions.at(0).get());
        const auto& expected = ast::assign_expr
        (
            std::move(std::make_unique<ast::identifier>("var_0", 0)),
            std::move(std::unique_ptr<ast::literal<u64>>(new ast::literal<u64>(1)))
        );
        ASSERT_EQ(actual, expected);
    }
}
