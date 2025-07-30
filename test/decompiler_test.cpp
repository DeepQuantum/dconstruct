#include "decompiler.h"
#include "binaryfile.h"
#include "file_disassembler.h"
#include "binary_expressions.h"
#include "literal.h"
#include "statements.h"
#include "assign_statement.h"
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

    dcompiler::expression_frame make_expression_frame(const std::vector<Instruction>& istrs) {
        StackFrame sf{};
        std::vector<function_disassembly_line> lines{};
        for (u64 i = 0; i < istrs.size(); ++i) {
            lines.push_back(function_disassembly_line(i, istrs.data()));
        }
        function_disassembly fd{lines, std::move(sf), "Test"};
        dcompiler::Decompiler dc(&fd);
        std::vector<dcompiler::decompiled_function> funcs = dc.decompile();
        return std::move(funcs[0].m_frame);
    }

     TEST(DECOMPILER, BasicLoadImmediate) {
         const std::vector<Instruction> istrs = {
             {Opcode::LoadU16Imm, 0, 1, 0},
         };
         const dcompiler::expression_frame frame = make_expression_frame(istrs);

         const auto& actual = *dynamic_cast<const ast::assign_statement*>(frame.m_statements[0].get());
         const auto& expression = std::make_unique<ast::assign_expr>(
             std::make_unique<ast::identifier>("var_0", 0),
             std::unique_ptr<ast::literal>(new ast::literal(1ULL))
         );
         const auto& expected = ast::assign_statement(ast::TK_U64, expression.get());
         ASSERT_EQ(actual, expected);
         ASSERT_EQ(frame.m_typedExpressions.at(0).m_type, ast::TK_U64);
     }

    TEST(DECOMPILER, BasicLoadImmediateString) {
        const dcompiler::expression_frame frame = make_expression_frame({
            {Opcode::LoadU16Imm, 0, 1, 0},
        });

        const auto& actual = *dynamic_cast<const ast::assign_statement*>(frame.m_statements[0].get());
        const auto& expression = std::make_unique<ast::assign_expr>(
            std::unique_ptr<ast::identifier>(new ast::identifier(0)),
            std::unique_ptr<ast::literal>(new ast::literal(1ULL))
        );
        std::ostringstream actual_os, expected_os;
        actual.pseudo(actual_os);
        const auto expected = ast::assign_statement(ast::TK_U64, expression.get());
        expected.pseudo(expected_os);
        ASSERT_EQ(actual_os.str(), expected_os.str());
        ASSERT_EQ(frame.m_typedExpressions.at(0).m_type, ast::TK_U64);
    }


    TEST(DECOMPILER, BasicLoadImmediatesString) {
        const std::vector<Instruction> istrs = {
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::LoadU16Imm, 2, 5, 5},
        };
        const dcompiler::expression_frame frame = make_expression_frame(istrs);

        const auto& actual = frame.m_statements;
        const std::string expected = "i32 var_0 = 1;\ni32 var_1 = 1285;\n";
        std::ostringstream os;
        for (const auto& stmt : actual) {
            stmt->pseudo(os);
        }
        EXPECT_EQ(expected, os.str());
    }


    TEST(DECOMPILER, BasicIdentifierAdd) {
        const std::vector<Instruction> istrs = {
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::LoadU16Imm, 1, 5, 5},
            {Opcode::IAdd, 0, 0, 1}
        };
        const dcompiler::expression_frame frame = make_expression_frame(istrs);

        const auto& actual = frame.m_statements;
        const std::string expected = "i32 var_0 = 1;\ni32 var_1 = 1285;\ni32 var_2 = var_0 + var_1;\n";
        std::ostringstream os;
        for (const auto& stmt : actual) {
            stmt->pseudo(os);
        }
        EXPECT_EQ(expected, os.str());
    }
}
