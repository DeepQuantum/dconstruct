#include <gtest/gtest.h>
#include "compilation/lexer.h"
#include "compilation/dc_parser.h"
#include "compilation/function.h"
#include "disassembly/file_disassembler.h"
#include "disassembly/instructions.h"
#include "decompilation/decomp_function.h"
#include "binaryfile.h"
#include "ast/ast.h"
#include <vector>
#include <fstream>
#include <algorithm>

namespace dconstruct::testing {
    
const std::string TEST_DIR = "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/dc_test_files/";
const std::string DCPL_PATH = "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/dcpl/";

    static SIDBase base = *SIDBase::from_binary(TEST_DIR + R"(\test_sidbase.bin)");

    static std::pair<std::vector<compilation::token>, std::vector<compilation::lexing_error>> get_tokens(const std::string &string) {
        dconstruct::compilation::Lexer lexer = dconstruct::compilation::Lexer(string);
        return { lexer.scan_tokens(), lexer.get_errors() };
    } 

    static std::tuple<ast::program, std::unordered_map<std::string, ast::full_type>, std::vector<compilation::parsing_error>> get_parse_results(const std::vector<compilation::token> &tokens) {
        compilation::Parser parser{tokens};
        return { parser.parse(), parser.get_known_types(), parser.get_errors() };
    }

    static std::pair<std::list<stmnt_uptr>, std::vector<compilation::parsing_error>> get_statements(std::vector<compilation::token> &tokens) {
        if (tokens.back().m_type == compilation::token_type::_EOF) {
            tokens.pop_back();
        }

        std::vector<compilation::token> function_def_tokens {
            compilation::token(compilation::token_type::IDENTIFIER, "i32", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "main", 0, 1),
            compilation::token(compilation::token_type::LEFT_PAREN, "(", 0, 1),
            compilation::token(compilation::token_type::RIGHT_PAREN, ")", 0, 1),
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 1),
        };

        function_def_tokens.insert(function_def_tokens.end(), tokens.begin(), tokens.end());


        function_def_tokens.push_back(compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 1));
        function_def_tokens.push_back(compilation::token(compilation::token_type::_EOF, "", 0, 1));

        compilation::Parser parser{function_def_tokens};

        auto [program, _, errors] = get_parse_results(function_def_tokens);

        return { !program.m_declarations.empty() ? std::move(static_cast<ast::function_definition*>(program.m_declarations[0].get())->m_body.m_statements) : std::list<stmnt_uptr>{}, errors };
    }


    [[nodiscard]] static std::expected<std::vector<compilation::function>, std::string> compile_to_functions(const std::string& code) {
        const auto [tokens, lex_errors] = get_tokens(code);
        if (!lex_errors.empty()) {
            return std::unexpected{"lex errors: " + lex_errors.front().m_message};
        }
        auto [program, types, parse_errors] = get_parse_results(tokens);
        if (!parse_errors.empty()) {
            return std::unexpected{"parse errors: " + parse_errors.front().m_message};
        }
        compilation::scope scope{types};
        const auto semantic_errors = program.check_semantics(scope);
        if (!semantic_errors.empty()) {
            return std::unexpected{"semantic errors: " + semantic_errors.front().m_message};
        }
        auto functions = program.compile_functions(scope);
        if (!functions) {
            return std::unexpected{functions.error()};
        }
        return *functions;
    }

    static void expect_instructions(const std::string& code, const std::vector<Instruction>& expected) {
        auto functions = compile_to_functions(code);
        ASSERT_TRUE(functions) << "compile failed: " << (functions ? "" : functions.error());
        ASSERT_FALSE(functions->empty()) << "no functions produced";
        EXPECT_EQ((*functions)[0].m_instructions, expected);
    }

    TEST(COMPILER, LexerEmpty) {
        const std::string empty = "";
        const auto [tokens, errors] = get_tokens(empty);
        const compilation::token eof = compilation::token(compilation::token_type::_EOF, "", 0, 1);
        
        EXPECT_EQ(tokens.back(), eof);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, WrongStatementCast) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 1),
            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);

        const ast::variable_declaration* actual = dynamic_cast<const ast::variable_declaration*>(statements.front().get());
        EXPECT_EQ(actual, nullptr);
    }

    TEST(COMPILER, WrongExpressionCast) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 1),
            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        const ast::block* block = static_cast<const ast::block*>(statements.front().get());
        const ast::expression_stmt* expr = static_cast<const ast::expression_stmt*>(block->m_statements.front().get());
        const ast::binary_expr* bin_expr = dynamic_cast<const ast::binary_expr*>(expr->m_expression.get());

        EXPECT_EQ(bin_expr, nullptr);
    }

    TEST(COMPILER, LexerBasicChars) {
        const std::string chars = "+.{)";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            compilation::token(compilation::token_type::DOT, ".", 0, 1),
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 1),
            compilation::token(compilation::token_type::RIGHT_PAREN, ")", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerDoubleChars) {
        const std::string chars = "!=,==}<==";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::BANG_EQUAL, "!=", 0, 1),
            compilation::token(compilation::token_type::COMMA, ",", 0, 1),
            compilation::token(compilation::token_type::EQUAL_EQUAL, "==", 0, 1),
            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 1),
            compilation::token(compilation::token_type::LESS_EQUAL, "<=", 0, 1),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerInvalidCharacter) {
        const std::string chars = "@";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };

        const std::vector<compilation::lexing_error> expected_errors = {
            compilation::lexing_error(1, "invalid token '@'")
        };

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors, expected_errors);
    }

    TEST(COMPILER, LexerComment) {
        const std::string chars = "/=.//daaindasd}\n+";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::SLASH_EQUAL, "/=", 0, 1),
            compilation::token(compilation::token_type::DOT, ".", 0, 1),
            compilation::token(compilation::token_type::PLUS, "+", 0, 2),
            compilation::token(compilation::token_type::_EOF, "", 0, 2),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerString) {
        const std::string chars = "++\"blackeyeGalaxy_abc\"++";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::PLUS_PLUS, "++", 0, 1),
            compilation::token(compilation::token_type::STRING, "\"blackeyeGalaxy_abc\"", "blackeyeGalaxy_abc", 1),
            compilation::token(compilation::token_type::PLUS_PLUS, "++", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerStringEOF) {
        const std::string chars = "\"blackeyeGalaxy_abc";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };

        const std::vector<compilation::lexing_error> expected_errors = {
            compilation::lexing_error(1, "unterminated string literal")
        };

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(expected_errors, errors);
    }

    TEST(COMPILER, LexerSimpleInt) {
        const std::string chars = "]{123\n34";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::RIGHT_SQUARE, "]", 0, 1),
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 1),
            compilation::token(compilation::token_type::INT, "123", static_cast<u16>(123), 1),
            compilation::token(compilation::token_type::INT, "34", static_cast<u16>(34), 2),
            compilation::token(compilation::token_type::_EOF, "", 0, 2),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerSimpleDouble) {
        const std::string chars = "123.45\"a\"";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::DOUBLE, "123.45", 123.45f, 1),
            compilation::token(compilation::token_type::STRING, "\"a\"", "a", 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerSimpleHex) {
        const std::string chars = "0x123.abc";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::INT, "0x123", (u16)0x123, 1),
            compilation::token(compilation::token_type::DOT, ".", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "abc", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };
        
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerSimpleSID) {
        const std::string chars = "1.3-=#ellie";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::DOUBLE, "1.3", 1.3f, 1),
            compilation::token(compilation::token_type::MINUS_EQUAL, "-=", 0, 1),
            compilation::token(compilation::token_type::SID, "#ellie", sid64_literal(0,"ellie"), 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }


    TEST(COMPILER, LexerSimpleIdentifier) {
        const std::string chars = "abc\ndef;";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::IDENTIFIER, "abc", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "def", 0, 2),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 2),
            compilation::token(compilation::token_type::_EOF, "", 0, 2),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerSimpleKeywords) {
        const std::string chars = "return;";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::RETURN, "return", 0, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerProgram1) {
        const std::string chars = 
            "int main()\n" 
            "{" 
            "\tint a = 0;" 
            "\ta += 1;" 
            "\treturn a;"
            "}";
        
        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::IDENTIFIER, "int", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "main", 0, 1),
            compilation::token(compilation::token_type::LEFT_PAREN, "(", 0, 1),
            compilation::token(compilation::token_type::RIGHT_PAREN, ")", 0, 1),
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 2),
            compilation::token(compilation::token_type::IDENTIFIER, "int", 0, 3),
            compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 3),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 3),
            compilation::token(compilation::token_type::INT, "0", 0, 3),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 3),
            compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 4),
            compilation::token(compilation::token_type::PLUS_EQUAL, "+=", 0, 4),
            compilation::token(compilation::token_type::INT, "1", 0, 4),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 4),
            compilation::token(compilation::token_type::RETURN, "return", 0, 5),
            compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 5),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 5),
            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 6),
            compilation::token(compilation::token_type::_EOF, "", 0, 6),
        };
    }

    TEST(COMPILER, LexerComplexSids) {
        const std::string chars = "#simple_sid #%alloc-array? #=f(test123)";
        const auto [tokens, errors] = get_tokens(chars);
        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::SID, "#simple_sid", sid64_literal(0, "simple_sid"), 1),
            compilation::token(compilation::token_type::SID, "#%alloc-array?", sid64_literal(0, "%alloc-array?"), 1),
            compilation::token(compilation::token_type::SID, "#=f", sid64_literal(0, "=f"), 1),
            compilation::token(compilation::token_type::LEFT_PAREN, "(", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "test123", 0, 1),
            compilation::token(compilation::token_type::RIGHT_PAREN, ")", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };

        ASSERT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerProgramWithExtendedFeatures) {
        const std::string chars = 
            "struct Person {\n"
            "\tsid name = #ellie;\n"
            "\tdouble damage = 5.9;\n"
            "\tint hexVal = 0x1A3F;\n"
            "\tstring msg = \"Hello, world!\";\n"
            "\tif (damage > 6.0) {\n"
            "\t\tmsg = \"Joel Miller\";\n"
            "\t} else {\n"
            "\t\tmsg = \"Test String\";\n"
            "\t}\n"
            "};"
            ;

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::STRUCT, "struct", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "Person", 0, 1),
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 1),

            compilation::token(compilation::token_type::IDENTIFIER, "sid", 0, 2),
            compilation::token(compilation::token_type::IDENTIFIER, "name", 0, 2),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 2),
            compilation::token(compilation::token_type::SID, "#ellie", sid64_literal(0, "ellie"), 2),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 2),

            compilation::token(compilation::token_type::IDENTIFIER, "double", 0, 3),
            compilation::token(compilation::token_type::IDENTIFIER, "damage", 0, 3),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 3),
            compilation::token(compilation::token_type::DOUBLE, "5.9", 5.9f, 3),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 3),

            compilation::token(compilation::token_type::IDENTIFIER, "int", 0, 4),
            compilation::token(compilation::token_type::IDENTIFIER, "hexVal", 0, 4),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 4),
            compilation::token(compilation::token_type::INT, "0x1A3F", (u16)0x1A3F, 4),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 4),

            compilation::token(compilation::token_type::IDENTIFIER, "string", 0, 5),
            compilation::token(compilation::token_type::IDENTIFIER, "msg", 0, 5),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 5),
            compilation::token(compilation::token_type::STRING, "\"Hello, world!\"", "Hello, world!", 5),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 5),

            compilation::token(compilation::token_type::IF, "if", 0, 6),
            compilation::token(compilation::token_type::LEFT_PAREN, "(", 0, 6),
            compilation::token(compilation::token_type::IDENTIFIER, "damage", 0, 6),
            compilation::token(compilation::token_type::GREATER, ">", 0, 6),
            compilation::token(compilation::token_type::DOUBLE, "6.0", 6.0f, 6),
            compilation::token(compilation::token_type::RIGHT_PAREN, ")", 0, 6),
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 6),

            compilation::token(compilation::token_type::IDENTIFIER, "msg", 0, 7),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 7),
            compilation::token(compilation::token_type::STRING, "\"Joel Miller\"", "Joel Miller", 7),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 7),

            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 8),
            compilation::token(compilation::token_type::ELSE, "else", 0, 8),
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 8),

            compilation::token(compilation::token_type::IDENTIFIER, "msg", 0, 9),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 9),
            compilation::token(compilation::token_type::STRING, "\"Test String\"", "Test String", 9),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 9),

            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 10),

            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 11),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 11),
            compilation::token(compilation::token_type::_EOF, "", 0, 11),
        };

        const auto [tokens, errors] = get_tokens(chars);
        for (u32 i = 0; i < tokens.size(); ++i) {
            ASSERT_EQ(tokens[i], expected[i]);
        }
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, SimpleNumParse1) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::INT, "1", 1, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 1, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1) 
        };

        const auto [statements, errors] = get_statements(tokens);
        
        const ast::literal expected = ast::literal(1);

        const ast::expression& actual = *dynamic_cast<const ast::expression_stmt*>(statements.front().get())->m_expression;

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, SimpleNumParse2) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::INT, "1", 1, 1),
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            compilation::token(compilation::token_type::INT, "2", 2, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 2, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const ast::expression& actual = *dynamic_cast<const ast::expression_stmt*>(statements.front().get())->m_expression;
        
        const ast::add_expr expected{
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            std::make_unique<ast::literal>(1),
            std::make_unique<ast::literal>(2)
        };

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, SimpleNumParse3) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::INT, "1", 1, 1),
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            compilation::token(compilation::token_type::INT, "2", 2, 1),
            compilation::token(compilation::token_type::STAR, "*", 0, 1),
            compilation::token(compilation::token_type::INT, "5", 5, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 5, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const ast::expression& actual = *dynamic_cast<const ast::expression_stmt*>(statements.front().get())->m_expression;
        
        expr_uptr left = std::make_unique<ast::mul_expr>(
            compilation::token(compilation::token_type::STAR, "*", 0, 1),
            std::make_unique<ast::literal>(2),
            std::make_unique<ast::literal>(5)
        );

        const ast::add_expr expected{
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            std::make_unique<ast::literal>(1),
            std::move(left)
        };

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    /*TEST(COMPILER, SimpleNumParseError1) {
        std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::INT, "1", 1, 1),
            compiler::token(compiler::token_type::PLUS, "+", 0, 1),
            compiler::token(compiler::token_type::INT, "2", 2, 1),
            compiler::token(compiler::token_type::STAR, "*", 0, 1),
            compiler::token(compiler::token_type::INT, "5", 5, 1),
            compiler::token(compiler::token_type::MINUS, "-", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);
        EXPECT_EQ(statements.size(), 0);
        EXPECT_EQ(errors.size(), 1);
        EXPECT_EQ(errors[0].m_message, "error: expected expression after '-'");
    }*/

    TEST(COMPILER, GroupNumParse) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::LEFT_PAREN, "(", 1, 1),
            compilation::token(compilation::token_type::INT, "1", 1, 1),
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            compilation::token(compilation::token_type::INT, "2", 2, 1),
            compilation::token(compilation::token_type::RIGHT_PAREN, ")", 1, 1),
            compilation::token(compilation::token_type::STAR, "*", 0, 1),
            compilation::token(compilation::token_type::INT, "5", 5, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 5, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const ast::expression& actual = *dynamic_cast<const ast::expression_stmt*>(statements.front().get())->m_expression;
        
        auto left = std::make_unique<ast::grouping>(std::make_unique<ast::add_expr>(
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            std::make_unique<ast::literal>(1),
            std::make_unique<ast::literal>(2)
        ));

        const ast::mul_expr expected{
            compilation::token(compilation::token_type::STAR, "*", 0, 1),
            std::move(left),
            std::make_unique<ast::literal>(5)
        };

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, VariableDeclaration0) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::IDENTIFIER, "u16", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "number", 0, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const auto& actual = *static_cast<const ast::variable_declaration*>(statements.front().get());

        const ast::variable_declaration expected{ast::make_type_from_prim(ast::primitive_kind::U16), "number"};

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, VariableDeclaration1) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::IDENTIFIER, "u16", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "number", 0, 1),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 1),
            compilation::token(compilation::token_type::INT, "2", 2, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const auto& actual = *static_cast<const ast::variable_declaration*>(statements.front().get());

        const ast::variable_declaration expected{ast::make_type_from_prim(ast::primitive_kind::U16), "number", 2};

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, SimpleBlock) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 1),
            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const ast::block& actual = *dynamic_cast<const ast::block*>(statements.front().get());

        std::list<stmnt_uptr> expected_statements{};
        expected_statements.push_back(std::move(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::identifier>(compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 1))
        )));

        const ast::block expected{std::move(expected_statements)};

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, SimpleBlockError) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "b", 0, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 1),
            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const ast::block& actual = *dynamic_cast<const ast::block*>(statements.front().get());

        std::list<stmnt_uptr> expected_statements{};
        expected_statements.push_back(std::move(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::identifier>(compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 1))
        )));

        const ast::block expected{std::move(expected_statements)};

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_NE(expected, actual);
    }

    TEST(COMPILER, Program1) {
        const std::string code = "i32 x = 0; i32 y = 1; while (x < y) { x = x + y; }";

        auto [tokens, lex_errors] = get_tokens(code);

        const auto [statements, parse_errors] = get_statements(tokens);

        EXPECT_EQ(statements.size(), 3);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);

        std::list<stmnt_uptr> expected;

        std::list<stmnt_uptr> block_stmnt;

        block_stmnt.push_back(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::assign_expr>(
				std::make_unique<ast::identifier>("x"),
                std::make_unique<ast::add_expr>(
                    compilation::token(compilation::token_type::PLUS, "+", 0, 1),
                    std::make_unique<ast::identifier>("x"),
                    std::make_unique<ast::identifier>("y")
                )
            )
        ));
        expected.push_back(std::make_unique<ast::variable_declaration>(ast::make_type_from_prim(ast::primitive_kind::U32), "x", 0));
        expected.push_back(std::make_unique<ast::variable_declaration>(ast::make_type_from_prim(ast::primitive_kind::U32), "y", 1));
        expected.push_back(std::make_unique<ast::while_stmt>(
            std::make_unique<ast::compare_expr>(
                compilation::token(compilation::token_type::LESS, "<", 0, 1),
                std::make_unique<ast::identifier>("x"),
                std::make_unique<ast::identifier>("y")
            ),
            std::make_unique<ast::block>(
                std::move(block_stmnt)
            )
        ));

        EXPECT_EQ(expected, statements);
    }

    TEST(COMPILER, OrderOfOperationsProgram) {
        const std::string code = "i32 x = 1 + 2 * 3 - 4 / 5;";

        auto [tokens, lex_errors] = get_tokens(code);

        const auto [statements, parse_errors] = get_statements(tokens);

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);

        std::list<stmnt_uptr> expected;

        expected.push_back(std::make_unique<ast::variable_declaration>(ast::make_type_from_prim(ast::primitive_kind::U32), "x",
            std::make_unique<ast::sub_expr>(
                compilation::token(compilation::token_type::MINUS, "-", 0, 1),
                std::make_unique<ast::add_expr>(
                    compilation::token(compilation::token_type::PLUS, "+", 0, 1),
                    std::make_unique<ast::literal>(1),
                    std::make_unique<ast::mul_expr>(
                        compilation::token(compilation::token_type::STAR, "*", 0, 1),
                        std::make_unique<ast::literal>(2),
                        std::make_unique<ast::literal>(3)
                    )
                ),
                std::make_unique<ast::div_expr>(
                    compilation::token(compilation::token_type::SLASH, "/", 0, 1),
                    std::make_unique<ast::literal>(4),
                    std::make_unique<ast::literal>(5)
                )
            )
        ));

        EXPECT_EQ(expected, statements);
    }

    TEST(COMPILER, Foreach1) {
        const std::string code = "u32 y = 0; foreach (u32 item : iterable) { y = y + item; }";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [statements, parse_errors] = get_statements(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(statements.size(), 2);
        std::list<stmnt_uptr> expected;
        expected.push_back(std::make_unique<ast::variable_declaration>(ast::make_type_from_prim(ast::primitive_kind::U32), "y", 0));
        std::list<stmnt_uptr> block_stmnts;
        block_stmnts.push_back(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::assign_expr>(
                std::make_unique<ast::identifier>("y"),
                std::make_unique<ast::add_expr>(
                    compilation::token(compilation::token_type::PLUS, "+", 0, 1),
                    std::make_unique<ast::identifier>("y"),
                    std::make_unique<ast::identifier>("item")
                )
            )
        ));
        expected.push_back(std::make_unique<ast::foreach_stmt>(
            ast::parameter{ast::make_type_from_prim(ast::primitive_kind::U32),"item",},
            std::make_unique<ast::identifier>("iterable"),
            std::make_unique<ast::block>(std::move(block_stmnts))
        ));
        EXPECT_EQ(expected, statements);
    }

    TEST(COMPILER, Match1) {
        const std::string code = "return match (var_0; var_1; var_2) { 0, 1 -> \"Cool\", 2 -> \"Not cool\", else -> \"Default\" };";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [statements, parse_errors] = get_statements(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(statements.size(), 1);
        
        std::vector<expr_uptr> vars;
        vars.push_back(std::make_unique<ast::identifier>("var_0"));
        vars.push_back(std::make_unique<ast::identifier>("var_1"));
        vars.push_back(std::make_unique<ast::identifier>("var_2"));

        std::vector<expr_uptr> case1_vals;
        case1_vals.push_back(std::make_unique<ast::literal>(0));
        case1_vals.push_back(std::make_unique<ast::literal>(1));

        std::vector<expr_uptr> case2_vals;
        case2_vals.push_back(std::make_unique<ast::literal>(2));

        std::vector<ast::match_expr::matches_t> outer;
        outer.emplace_back(std::move(case1_vals), std::make_unique<ast::literal>("Cool"));
        outer.emplace_back(std::move(case2_vals), std::make_unique<ast::literal>("Not cool"));

        std::list<stmnt_uptr> expected;
        expected.push_back(std::make_unique<ast::return_stmt>(
            std::make_unique<ast::match_expr>(
                std::move(vars),
                std::move(outer),
                std::make_unique<ast::literal>("Default")
            )
        ));

        EXPECT_EQ(expected, statements);
    }

    TEST(COMPILER, Call1) {
        const std::string code = "doSomething(1, 2 + 3);";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [statements, parse_errors] = get_statements(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(statements.size(), 1);

        std::vector<expr_uptr> args;
        args.push_back(std::make_unique<ast::literal>(1));
        args.push_back(std::make_unique<ast::add_expr>(
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            std::make_unique<ast::literal>(2),
            std::make_unique<ast::literal>(3)
        ));

        std::list<stmnt_uptr> expected;
        expected.push_back(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::call_expr>(
                compilation::token(compilation::token_type::IDENTIFIER, "doSomething", 0, 1),
                std::make_unique<ast::identifier>("doSomething"),
                std::move(args)
            )
        ));

        EXPECT_EQ(expected, statements);
    }

    TEST(COMPILER, Semantics1) {
        const std::string code = "if (1) { 2 / 1; }";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [statements, parse_errors] = get_statements(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(statements.size(), 1);

        std::list<stmnt_uptr> block;
        block.push_back(
            std::make_unique<ast::expression_stmt>(
                std::make_unique<ast::div_expr>(
                    compilation::token(compilation::token_type::SLASH, "/", 0, 1),
                    std::make_unique<ast::literal>(2),
                    std::make_unique<ast::literal>(1)
                )
            )
        );

        ast::if_stmt expected_if(
            std::make_unique<ast::literal>(1),
            std::make_unique<ast::block>(
                std::move(block)
            )
        );

        const auto& rhs = *dynamic_cast<ast::if_stmt*>(statements.front().get());
        EXPECT_EQ(expected_if, rhs);

        dconstruct::compilation::scope type_env{{}};
        const auto errors = statements.front()->check_semantics(type_env);

        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexError1) {
        const std::string chars = "@";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };

        const std::vector<compilation::lexing_error> expected_errors = {
            compilation::lexing_error(1, "invalid token '@'")
        };

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(expected_errors, errors);
    }

    TEST(COMPILER, ParseError1) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::INT, "1", 1, 1),
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            compilation::token(compilation::token_type::_EOF, ";", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        ASSERT_EQ(errors.size(), 2);

        const std::list<stmnt_uptr> expected_statements = {
        };
        EXPECT_EQ(statements, expected_statements);
    }

    TEST(COMPILER, ParseStructType) {
        const std::string code = 
        "struct Vector3 {\n"
        "\tf32 x;\n"
        "\tf32 y;\n"
        "\tf32 z;\n"
        "}";

        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(program.m_declarations.size(), 0);
        ast::struct_type expected_type;
        expected_type.m_name = "Vector3";
        expected_type.m_members["x"] = std::move(std::make_shared<ast::full_type>(ast::make_type_from_prim(ast::primitive_kind::F32)));
        expected_type.m_members["y"] = std::move(std::make_shared<ast::full_type>(ast::make_type_from_prim(ast::primitive_kind::F32)));
        expected_type.m_members["z"] = std::move(std::make_shared<ast::full_type>(ast::make_type_from_prim(ast::primitive_kind::F32)));
        EXPECT_TRUE(types.contains("Vector3"));
        EXPECT_TRUE(std::holds_alternative<ast::struct_type>(types.at("Vector3")));
        auto type = std::get<ast::struct_type>(types.at("Vector3"));
        EXPECT_EQ(type, expected_type);
    }

    TEST(COMPILER, ParseEnumType) {
        const std::string code = 
        "enum Color {\n"
        "\tRed,\n"
        "\tGreen,\n"
        "\tBlue\n"
        "}";

        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(program.m_declarations.size(), 0);
        ast::enum_type expected_type;
        expected_type.m_name = "Color";
        expected_type.m_enumerators.push_back("Red");
        expected_type.m_enumerators.push_back("Green");
        expected_type.m_enumerators.push_back("Blue");
        EXPECT_TRUE(types.contains("Color"));
        EXPECT_TRUE(std::holds_alternative<ast::enum_type>(types.at("Color")));
        auto type = std::get<ast::enum_type>(types.at("Color"));
        EXPECT_EQ(type, expected_type);
    }

    TEST(COMPILER, FullFunc1) {
        const std::string code = "struct Vector3 { f32 x; f32 y; f32 z; } enum Opcode { MOVE, LOAD, PUSH } i32 vector3_dist(Vector3 a) { return 1.0; }";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(program.m_declarations.size(), 1);
        
        compilation::scope scope{types};
        const auto semantic_errors = program.m_declarations[0]->check_semantics(scope);

        std::vector<ast::semantic_check_error> empty{};
        EXPECT_EQ(semantic_errors, empty);
    }

    TEST(COMPILER, Declaration1) {
        const std::string code = "i32 main () { i32 x = 0; i32 y = 0; return x = y; }";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(program.m_declarations.size(), 1);
        
        compilation::scope scope{types};
        const auto semantic_errors = program.m_declarations[0]->check_semantics(scope);

        std::vector<ast::semantic_check_error> empty{};
        EXPECT_EQ(semantic_errors, empty);
    }

    TEST(COMPILER, Using1) {
        const std::string code = 
        "using far #display as (string, i32) -> void;"
        "using far #5445173390656D6D as (string, i32, i32) -> string sprintf;"
        "i32 main() {"
        "    string message = sprintf(\"Hello World from DC version %d.%d\", 0, 0);"
        "    display(message, 19);"
        "    return 0;"
        "}";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(program.m_declarations.size(), 3);

        compilation::scope scope{types};
        std::vector<ast::semantic_check_error> semantic_errors = program.check_semantics(scope);
        ASSERT_EQ(scope.m_sidAliases.at("sprintf").first, 0x5445173390656D6D);

        std::vector<ast::semantic_check_error> empty{};
        EXPECT_EQ(semantic_errors, empty);
    }

    TEST(COMPILER, FullCompile1) {
        const std::string code = 
            "i32 main() {"
            "   u64 x = 1;"
            "   u64 y = x * 2;"
            "}";

        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(program.m_declarations.size(), 1);

        compilation::scope scope{types};
        std::vector<ast::semantic_check_error> semantic_errors = program.check_semantics(scope);
        ASSERT_EQ(semantic_errors.size(), 0);

        const auto binary = program.compile_to_file(scope);
        ASSERT_TRUE(binary.has_value());
        const auto& [bytes, size] = *binary;
        std::ofstream out("compiled.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(bytes.get()), size);
        out.flush();
    }


    TEST(COMPILER, FullCompile2) {
        const std::string code = 
            "using far #display as (string, string, u64) -> void;"
            "using far #5445173390656D6D as (string, u64, u64) -> string sprintf;"
            "u32 main() {"
            "    display(\"test\", sprintf(\"Hello World from DC version %d.%d\", 0, 0), 19);"
            "    return 0;"
            "}";
        
        const std::string decomp_code = 
            "u16 #CBF29CE484222325() {\n"
            "    display(\"test\", #5445173390656D6D(\"Hello World from DC version %d.%d\", 0, 0), 19);\n"
            "    return 0;\n"
            "}";


        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(program.m_declarations.size(), 3);

        compilation::scope scope{types};
        std::vector<ast::semantic_check_error> semantic_errors = program.check_semantics(scope);
        ASSERT_EQ(semantic_errors.size(), 0);

        const auto binary = program.compile_to_file(scope);
        ASSERT_TRUE(binary.has_value());
        const auto& [bytes, size] = *binary;
        std::ofstream out("compiled.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(bytes.get()), size);
        out.flush();

        auto check_file = *BinaryFile::from_path("compiled.bin");
        Disassembler da{ &check_file, &base };
        da.disassemble();
        const auto& function = da.get_functions()[0];
        auto fd = dcompiler::decomp_function{function, check_file, ControlFlowGraph::build(function)};
        const std::string res = fd.decompile(true).to_c_string();
        ASSERT_EQ(res, decomp_code);
    }

    TEST(COMPILER, FullCompile3) {
        const std::string code = 
            "u32 main() {"
            "    u64 i = 1;\n"
            "    u64 res = 1;\n"
            "    while (i < 10) {\n"
            "        res = res * i;\n"
            "        i = i + 1;\n"
            "        while (i < 10) {"
            "           i = i + 2 * 3;"
            "        }"
            "    }\n"
            "    return res;\n"
            "}";
        
        const std::string decomp_code = 
            "u16 main() {"
            "    u16 var_0 = 1;\n"
            "    u64 var_1 = 1;\n"
            "    while (i < 10) {\n"
            "        res = res * i;\n"
            "        i = i + 1;\n"
            "    }\n"
            "    return i;\n"
            "}";


        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(program.m_declarations.size(), 3);

        compilation::scope scope{types};
        std::vector<ast::semantic_check_error> semantic_errors = program.check_semantics(scope);
        ASSERT_EQ(semantic_errors.size(), 0);

        const auto binary = program.compile_to_file(scope);
        ASSERT_TRUE(binary.has_value());
        const auto& [bytes, size] = *binary;
        std::ofstream out("compiled.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(bytes.get()), size);
        out.flush();

        auto check_file = *BinaryFile::from_path("compiled.bin");
        Disassembler da{ &check_file, &base };
        da.disassemble();
        const auto& function = da.get_functions()[0];
        auto fd = dcompiler::decomp_function{function, check_file, ControlFlowGraph::build(function)};
        const std::string res = fd.decompile(true).to_c_string();
        ASSERT_EQ(res, decomp_code);
    }

    TEST(COMPILER, FullCompile4) {
        const std::string code = 
            "u32 threexplusone(u64 x) {"
            "        if (x > 2) {"
            "            x = x / 2;"
            "        } else {"
            "            x = 3 * x + 1;"
            "        }"
            "}";
        
        const std::string decomp_code = 
            "u32 threexplusone(u64 x) {"
            "    while (x != 1) {\n"
            "        if (x % 2 == 0) {"
            "            x = x / 2;"
            "        } else {"
            "            x = 3 * i + 1;"
            "        }"
            "    }\n"
            "    return res;\n"
            "}";

        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(program.m_declarations.size(), 1);

        compilation::scope scope{types};
        std::vector<ast::semantic_check_error> semantic_errors = program.check_semantics(scope);
        ASSERT_EQ(semantic_errors.size(), 0);

        const auto binary = program.compile_to_file(scope);
        ASSERT_TRUE(binary.has_value());
        const auto& [bytes, size] = *binary;
        std::ofstream out("compiled.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(bytes.get()), size);
        out.flush();

        auto check_file = *BinaryFile::from_path("compiled.bin");
        Disassembler da{ &check_file, &base };
        da.disassemble();
        const auto& function = da.get_functions()[0];
        auto fd = dcompiler::decomp_function{function, check_file, ControlFlowGraph::build(function)};
        const std::string res = fd.decompile(true).to_c_string();
        ASSERT_EQ(res, decomp_code);
    }

    TEST(COMPILER, FullCompile5) {
        const std::string code =
            "u64 sum_upto(u64 x) {"
            "    u64 acc = 0;"
            "    for (u64 i = 0; i <= x; i++) {"
            "        acc = acc + i;"
            "    }"
            "    if (acc > 0xFFFFFFFF) {"
            "        acc = 0.0;"
            "    } else {"
            "        acc = 1.0;"
            "    }"
            "    return acc;"
            "}";
        
        const std::string decomp_code = "";

        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(program.m_declarations.size(), 1);

        compilation::scope scope{types};
        std::vector<ast::semantic_check_error> semantic_errors = program.check_semantics(scope);
        ASSERT_EQ(semantic_errors.size(), 0);

        const auto binary = program.compile_to_file(scope);
        ASSERT_TRUE(binary.has_value());
        const auto& [bytes, size] = *binary;
        std::ofstream out("compiled.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(bytes.get()), size);
        out.flush();

        auto check_file = *BinaryFile::from_path("compiled.bin");
        Disassembler da{ &check_file, &base };
        da.disassemble();
        const auto& function = da.get_functions()[0];
        auto fd = dcompiler::decomp_function{function, check_file, ControlFlowGraph::build(function)};
        const std::string res = fd.decompile(true).to_c_string();
        ASSERT_EQ(res, decomp_code);
    }

    TEST(COMPILER, InstructionLevel_CastExpression_ProducesExpectedInstructions) {
        // i32 main() { return (i32)1.0; }  ->  load float 1.0, CastInteger to int, move to return reg, return
        expect_instructions("i32 main() { return (i32)1.0; }", {
            Instruction{Opcode::LoadStaticFloatImm, 0, 0, 0},
            Instruction{Opcode::CastInteger, 1, 0, 0},
            Instruction{Opcode::Move, 0, 1, 0},
            Instruction{Opcode::Return, 0, 0, 0},
        });
    }

    TEST(COMPILER, InstructionLevel_Dereference_Load_EmitsLoadI32) {
        // i32 main(i32* p) { return *p; }  ->  Move param, LoadI32 from *p, Move to return reg, Return
        expect_instructions("i32 main(i32* p) { return *p; }", {
            Instruction{Opcode::Move, 0, 49, 0},
            Instruction{Opcode::LoadI32, 1, 0, 0},
            Instruction{Opcode::Move, 0, 1, 0},
            Instruction{Opcode::Return, 0, 0, 0},
        });
    }

    TEST(COMPILER, InstructionLevel_Dereference_Store_EmitsStoreI32) {
        // i32 main(i32* p) { *p = 42; return 0; }  ->  Move param, load 42, StoreI32, load 0, return
        expect_instructions("i32 main(i32* p) { *p = 42; return 0; }", {
            Instruction{Opcode::Move, 0, 49, 0},
            Instruction{Opcode::LoadStaticU64Imm, 1, 0, 0},
            Instruction{Opcode::StoreI32, 0, 1, 0},
            Instruction{Opcode::LoadU16Imm, 2, 0, 0},
            Instruction{Opcode::Move, 0, 2, 0},
            Instruction{Opcode::Return, 0, 0, 0},
        });
    }

    TEST(COMPILER, InstructionLevel_Dereference_Load_EmitsLoadU64) {
        // u64 main(u64* p) { return *p; }
        expect_instructions("u64 main(u64* p) { return *p; }", {
            Instruction{Opcode::Move, 0, 49, 0},
            Instruction{Opcode::LoadU64, 1, 0, 0},
            Instruction{Opcode::Move, 0, 1, 0},
            Instruction{Opcode::Return, 0, 0, 0},
        });
    }

    TEST(COMPILER, InstructionLevel_Dereference_Store_EmitsStoreU64) {
        // u64 main(u64* p) { *p = 42; return 0; }
        expect_instructions("u64 main(u64* p) { *p = 42; return 0; }", {
            Instruction{Opcode::Move, 0, 49, 0},
            Instruction{Opcode::LoadStaticU64Imm, 1, 0, 0},
            Instruction{Opcode::StoreU64, 0, 1, 0},
            Instruction{Opcode::LoadU16Imm, 2, 0, 0},
            Instruction{Opcode::Move, 0, 2, 0},
            Instruction{Opcode::Return, 0, 0, 0},
        });
    }
}
