#include <gtest/gtest.h>
#include "compilation/lexer.h"
#include "compilation/dc_parser.h"
#include "disassembly/disassembler.h"
#include "disassembly/instructions.h"
#include "decompilation/decomp_function.h"
#include "binaryfile.h"
#include "ast/ast.h"
#include <expected>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace dconstruct::testing {

    const std::filesystem::path DECOMP_TEST_ROOT = DCONSTRUCT_TEST_ROOT;
    const std::string DECOMP_FIXTURE_DIR = (DECOMP_TEST_ROOT / "fixtures" / "dc").string() + "/";

    struct decomp_case {
        std::vector<Instruction> m_instructions;
        std::string m_name = "test-function";
        std::string m_typeMap{};
        std::optional<ast::function_type> m_knownFunctionType{};
        std::vector<u64> m_symbolTable{};
        dcompiler::OPTIMIZATION_KIND m_optimizations = dcompiler::OPTIMIZATION_KIND::NONE;
    };

    struct decomp_result {
        ast::function_definition m_function;
        std::optional<std::string> m_error;

        [[nodiscard]] std::string pseudo_c() const {
            return m_function.to_pseudo_c_string();
        }
    };

    [[nodiscard]] static std::expected<ast::function_to_mapped_vars, std::string> parse_type_map(const std::string& source) {
        compilation::Lexer lexer{source};
        const auto [tokens, lex_errors] = lexer.get_results();
        if (!lex_errors.empty()) {
            return std::unexpected{"lex error: " + lex_errors.front().m_message};
        }
        compilation::Parser parser{tokens};
        std::optional<ast::function_to_mapped_vars> typemap = parser.make_typemap();
        if (!typemap || !parser.get_errors().empty()) {
            std::string message = "parse errors:";
            for (const auto& error : parser.get_errors()) {
                message += " " + error.m_message;
            }
            return std::unexpected{message};
        }
        return std::move(*typemap);
    }

    [[nodiscard]] static decomp_result run_decompilation(decomp_case test) {
        static SIDBase sidbase = *SIDBase::from_binary(DECOMP_FIXTURE_DIR + "test_sidbase.bin");
        BinaryFile file = *BinaryFile::from_path(DECOMP_FIXTURE_DIR + "dummy.bin");

        ast::function_to_mapped_vars scopes;
        if (!test.m_typeMap.empty()) {
            auto scopes_res = parse_type_map(test.m_typeMap);
            if (!scopes_res) {
                ADD_FAILURE() << scopes_res.error();
                return {};
            }
            scopes = std::move(*scopes_res);
        }

        const sid64 function_sid = SID(test.m_name.c_str());

        const ast::mapped_var_scope* function_scope = nullptr;
        if (const auto entry = scopes.find(function_sid); entry != scopes.end()) {
            function_scope = &entry->second;
        }

        const ast::function_type* known_function_type = nullptr;
        if (test.m_knownFunctionType) {
            known_function_type = &*test.m_knownFunctionType;
        } else if (const auto global_entry = scopes.find(SID("global")); global_entry != scopes.end()) {
            if (const auto entry = global_entry->second.find(function_sid); entry != global_entry->second.end()) {
                known_function_type = std::get_if<ast::function_type>(&entry->second.first);
            }
        }

        const std::shared_ptr<function_disassembly> fd = Disassembler::create_function_disassembly(
            test.m_instructions.data(),
            test.m_instructions.size(),
            test.m_name,
            location(test.m_symbolTable.data()),
            0,
            sizeof(Instruction),
            game_type::T2R,
            sidbase,
            file.m_sidCache
        );

        dcompiler::decomp_function decomp{
            *fd,
            file,
            sidbase,
            ControlFlowGraph::build(*fd),
            std::nullopt,
            nullptr,
            function_scope,
            known_function_type
        };
        ast::function_definition definition = decomp.decompile(test.m_optimizations);
        return {std::move(definition), std::move(decomp.m_error)};
    }

    TEST(DECOMPILER, ReturnLiteral) {
        decomp_case test{};
        test.m_instructions = {
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::Return, 0, 0, 0}};
        const decomp_result result = run_decompilation(std::move(test));

        EXPECT_FALSE(result.m_error.has_value());
        const std::string expected =
            "u16 test-function() {\n"
            "    return 1;\n"
            "}";
        EXPECT_EQ(expected, result.pseudo_c());
    }

    TEST(DECOMPILER, ReturnBinaryExpression) {
        decomp_case test{};
        test.m_instructions = {
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::LoadU16Imm, 1, 5, 5},
            {Opcode::IAdd, 2, 0, 1},
            {Opcode::Return, 2, 0, 0}};
        const decomp_result result = run_decompilation(std::move(test));

        EXPECT_FALSE(result.m_error.has_value());
        const std::string expected =
            "u16 test-function() {\n"
            "    return 1 + 1285;\n"
            "}";
        EXPECT_EQ(expected, result.pseudo_c());
    }

    TEST(DECOMPILER, RegisterArgumentTypes) {
        decomp_case test{};
        test.m_instructions = {
            {Opcode::Move, 0, 49, 0},
            {Opcode::LoadU16Imm, 1, 5, 0},
            {Opcode::IEqual, 2, 0, 1},
            {Opcode::Return, 2, 0, 0}};
        const decomp_result result = run_decompilation(std::move(test));

        EXPECT_FALSE(result.m_error.has_value());
        const std::string expected =
            "bool test-function(u16 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        EXPECT_EQ(expected, result.pseudo_c());
    }

    TEST(DECOMPILER, KnownFunctionTypeSetsParameterAndReturnTypes) {
        decomp_case test{};
        test.m_instructions = {
            {Opcode::Move, 0, 49, 0},
            {Opcode::LoadU16Imm, 1, 5, 0},
            {Opcode::IEqual, 2, 0, 1},
            {Opcode::Return, 2, 0, 0}};
        test.m_typeMap =
            "typemap {\n"
            "    i64 #test-function(i32 x) {\n"
            "    }\n"
            "}";
        const decomp_result result = run_decompilation(std::move(test));

        EXPECT_FALSE(result.m_error.has_value());
        const std::string expected =
            "i64 test-function(i32 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        EXPECT_EQ(expected, result.pseudo_c());

        ASSERT_EQ(result.m_function.m_parameters.size(), 1);
        ASSERT_TRUE(std::holds_alternative<ast::primitive_type>(result.m_function.m_parameters[0].m_type));
        EXPECT_EQ(std::get<ast::primitive_type>(result.m_function.m_parameters[0].m_type).m_type, ast::primitive_kind::I32);

        ASSERT_NE(result.m_function.m_type.m_return, nullptr);
        ASSERT_TRUE(std::holds_alternative<ast::primitive_type>(*result.m_function.m_type.m_return));
        EXPECT_EQ(std::get<ast::primitive_type>(*result.m_function.m_type.m_return).m_type, ast::primitive_kind::I64);
    }

    TEST(DECOMPILER, KnownFunctionTypeSetsArgumentIdentifierType) {
        decomp_case test{};
        test.m_instructions = {
            {Opcode::Move, 0, 49, 0},
            {Opcode::Return, 0, 0, 0}};
        test.m_typeMap =
            "typemap {\n"
            "    i32 #test-function(i32 x) {\n"
            "    }\n"
            "}";
        const decomp_result result = run_decompilation(std::move(test));

        EXPECT_FALSE(result.m_error.has_value());
        ASSERT_EQ(result.m_function.m_body.m_statements.size(), 1);
        const auto& return_stmt = static_cast<const ast::return_stmt&>(*result.m_function.m_body.m_statements.front());
        const std::optional<ast::full_type> identifier_type = return_stmt.m_expr->get_type();
        ASSERT_TRUE(identifier_type.has_value());
        ASSERT_TRUE(std::holds_alternative<ast::primitive_type>(*identifier_type));
        EXPECT_EQ(std::get<ast::primitive_type>(*identifier_type).m_type, ast::primitive_kind::I32);
    }

    TEST(DECOMPILER, KnownFunctionTypeUnknownArgumentFallsBackToRegisterArg) {
        decomp_case test{};
        test.m_instructions = {
            {Opcode::Move, 0, 49, 0},
            {Opcode::LoadU16Imm, 1, 5, 0},
            {Opcode::IEqual, 2, 0, 1},
            {Opcode::Return, 2, 0, 0}};
        ast::function_type known_type{};
        known_type.m_arguments.emplace_back("x", std::make_shared<ast::full_type>());
        test.m_knownFunctionType = std::move(known_type);
        const decomp_result result = run_decompilation(std::move(test));

        EXPECT_FALSE(result.m_error.has_value());
        const std::string expected =
            "bool test-function(u16 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        EXPECT_EQ(expected, result.pseudo_c());
    }

    TEST(DECOMPILER, KnownFunctionTypeArgumentCountMismatchSetsError) {
        decomp_case test{};
        test.m_instructions = {
            {Opcode::Move, 0, 49, 0},
            {Opcode::LoadU16Imm, 1, 5, 0},
            {Opcode::IEqual, 2, 0, 1},
            {Opcode::Return, 2, 0, 0}};
        test.m_typeMap =
            "typemap {\n"
            "    i64 #test-function(i32 x, i32 y) {\n"
            "    }\n"
            "}";
        const decomp_result result = run_decompilation(std::move(test));

        ASSERT_TRUE(result.m_error.has_value());
        EXPECT_NE(result.m_error->find("2 argument(s)"), std::string::npos);
        const std::string expected =
            "bool test-function(u16 arg_0) {\n"
            "    return arg_0 == 5;\n"
            "}";
        EXPECT_EQ(expected, result.pseudo_c());
    }

    TEST(DECOMPILER, KnownReturnTypeIsNotOverwrittenByReturnStatement) {
        decomp_case test{};
        test.m_instructions = {
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::Return, 0, 0, 0}};
        ast::function_type known_type{};
        known_type.m_return = std::make_shared<ast::full_type>(ast::make_type_from_prim(ast::primitive_kind::F32));
        test.m_knownFunctionType = std::move(known_type);
        const decomp_result result = run_decompilation(std::move(test));

        EXPECT_FALSE(result.m_error.has_value());
        ASSERT_NE(result.m_function.m_type.m_return, nullptr);
        ASSERT_TRUE(std::holds_alternative<ast::primitive_type>(*result.m_function.m_type.m_return));
        EXPECT_EQ(std::get<ast::primitive_type>(*result.m_function.m_type.m_return).m_type, ast::primitive_kind::F32);
    }

    TEST(DECOMPILER, KnownFunctionTypeIsNotMutatedByDecompilation) {
        decomp_case test{};
        test.m_instructions = {
            {Opcode::LoadU16Imm, 0, 1, 0},
            {Opcode::Return, 0, 0, 0}};
        ast::function_type known_type{};
        test.m_knownFunctionType = known_type;
        const decomp_result result = run_decompilation(test);

        EXPECT_FALSE(result.m_error.has_value());
        ASSERT_NE(result.m_function.m_type.m_return, nullptr);
        EXPECT_FALSE(is_unknown(*result.m_function.m_type.m_return));
        ASSERT_NE(test.m_knownFunctionType->m_return, nullptr);
        EXPECT_TRUE(is_unknown(*test.m_knownFunctionType->m_return));
    }

    TEST(DECOMPILER, VarMapAliasAndType) {
        decomp_case test{};
        test.m_instructions = {
            {Opcode::LookupPointer, 0, 0, 0},
            {Opcode::LoadU16Imm, 49, 5, 0},
            {Opcode::Call, 0, 0, 1},
            {Opcode::Return, 0, 0, 0}};
        test.m_symbolTable = {SID("ddict-key-count")};
        test.m_typeMap =
            "typemap {\n"
            "    #test-function {\n"
            "        0 i32 -> key_count;\n"
            "    }\n"
            "}";
        const decomp_result result = run_decompilation(std::move(test));

        EXPECT_FALSE(result.m_error.has_value());
        const std::string expected =
            "i32 test-function() {\n"
            "    i32 key_count = ddict-key-count(5);\n"
            "    return key_count;\n"
            "}";
        EXPECT_EQ(expected, result.pseudo_c());
    }
}
