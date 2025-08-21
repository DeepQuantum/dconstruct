#include "decompilation/decompiler.h"
#include "binaryfile.h"
#include "disassembly/file_disassembler.h"
#include "ast/ast.h"
#include <array>
#include <gtest/gtest.h>
#include <filesystem>

const std::string TEST_DIR = "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/dc_test_files/";

namespace dconstruct::testing {

    static function_disassembly get_function_disassembly(const std::string &path, const u32 offset) {
        SIDBase base{TEST_DIR + "test_sidbase.bin"}; 
        BinaryFile file(TEST_DIR + path); 
        file.dc_setup();
        FileDisassembler disassembler(&file, &base, "", DisassemblerOptions{}); 
        const ScriptLambda *lambda_ptr = disassembler.get_value_ptr_at<ScriptLambda>(offset); 
        const function_disassembly fd = disassembler.create_function_disassembly(lambda_ptr);
        return fd;
    }

    static dcompiler::expression_frame make_expression_frame(const std::vector<Instruction>& istrs) {
        StackFrame sf{};
        SIDBase base{R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\dc_test_files\test_sidbase.bin)"};
        std::vector<function_disassembly_line> lines{};
        for (u64 i = 0; i < istrs.size(); ++i) {
            lines.push_back(function_disassembly_line(i, istrs.data()));
        }
        function_disassembly fd{lines, std::move(sf), "Test"};
        dcompiler::Decompiler dc(&fd, base);
        std::vector<dcompiler::decompiled_function> funcs = dc.decompile();
        return std::move(funcs[0].m_frame);
    }

    TEST(DECOMPILER, BasicLoadImmediate) {
        const compiler::environment env{};
        const std::vector<Instruction> istrs = {
            {Opcode::LoadU16Imm, 0, 1, 0},
        };
        const dcompiler::expression_frame frame = make_expression_frame(istrs);

        const auto& actual = *static_cast<const ast::variable_declaration*>(frame.m_statements[0].get());
        const auto rhs = actual.get_init_ptr()->compute_type(env);

        ASSERT_TRUE(rhs.has_value());

        const auto& type = rhs.value();

        auto expected = ast::variable_declaration("u16", "var_0", u16(1));

        ASSERT_EQ(actual, expected);
    }

    TEST(DECOMPILER, BasicLoadImmediateString) {
        const compiler::environment env{};
        const dcompiler::expression_frame frame = make_expression_frame({
            {Opcode::LoadU16Imm, 0, 1, 0},
        });

        const auto& actual = *static_cast<const ast::variable_declaration*>(frame.m_statements[0].get());
        const auto rhs = actual.get_init_ptr()->compute_type(env);

        ASSERT_TRUE(rhs.has_value());

        const auto& type = rhs.value();

        const auto expected = ast::variable_declaration("u16", "var_0", u16(1));

        std::ostringstream actual_os, expected_os;
        actual.pseudo(actual_os);
        expected.pseudo(expected_os);
        const std::string actual_str = actual_os.str();
        const std::string expected_str = expected_os.str();
        ASSERT_EQ(expected_str, actual_str);
    }


    TEST(DECOMPILER, BasicLoadImmediatesString) {
        const std::vector<Instruction> istrs = {
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::LoadU16Imm, 2, 5, 5},
        };
        const dcompiler::expression_frame frame = make_expression_frame(istrs);

        const auto& actual = frame.m_statements;
        const std::string expected = "u16 var_0 = 1;\nu16 var_1 = 1285;\n";
        std::ostringstream os;
        for (const auto& stmt : actual) {
            stmt->pseudo(os);
            os << '\n';
        }
        EXPECT_EQ(expected, os.str());
    }


    TEST(DECOMPILER, BasicIdentifierAdd) {
        const u16 dest = 2;
        const std::vector<Instruction> istrs = {
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::LoadU16Imm, 1, 5, 5},
            {Opcode::IAdd, dest, 0, 1}
        };
        dcompiler::expression_frame frame = make_expression_frame(istrs);

        const auto& actual = frame.m_statements;
        frame.m_statements.push_back(std::make_unique<ast::variable_declaration>("u16", "var_2", std::move(frame.m_transformableExpressions[dest])));
        const std::string expected = "u16 var_0 = 1;\nu16 var_1 = 1285;\nu16 var_2 = var_0 + var_1;\n";
        std::ostringstream os;
        for (const auto& stmt : actual) {
            stmt->pseudo(os);
            os << '\n';
        }
        EXPECT_EQ(expected, os.str());
    }

    TEST(DECOMPILER, TwoAdds) {
        const u16 dest = 2;
        const std::vector<Instruction> istrs = {
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::LoadU16Imm, 1, 5, 5},
            {Opcode::IAdd, dest, 0, 1},
            {Opcode::IAdd, dest, 2, 2}
        };
        dcompiler::expression_frame frame = make_expression_frame(istrs);

        const auto& actual = frame.m_statements;
        frame.m_statements.push_back(std::make_unique<ast::variable_declaration>("u16", "var_2", std::move(frame.m_transformableExpressions[dest])));
        const std::string expected = "u16 var_0 = 1;\nu16 var_1 = 1285;\nu16 var_2 = (var_0 + var_1) + (var_0 + var_1);\n";

        std::ostringstream os;
        for (const auto& stmt : actual) {
            stmt->pseudo(os);
            os << '\n';
        }
        EXPECT_EQ(expected, os.str());
    }
}
