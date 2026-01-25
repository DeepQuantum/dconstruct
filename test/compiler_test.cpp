#include <gtest/gtest.h>
#include "compilation/lexer.h"
#include "compilation/dc_parser.h"
#include "ast/ast.h"
#include <vector>

namespace dconstruct::testing {
    static std::pair<std::vector<compiler::token>, std::vector<compiler::lexing_error>> get_tokens(const std::string &string) {
        dconstruct::compiler::Lexer lexer = dconstruct::compiler::Lexer(string);
        return { lexer.scan_tokens(), lexer.get_errors() };
    } 

    static std::tuple<std::vector<ast::global_decl_uptr>, std::unordered_map<std::string, ast::full_type>, std::vector<compiler::parsing_error>> get_parse_results(const std::vector<compiler::token> &tokens) {
        compiler::Parser parser{tokens};
        return { parser.parse(), parser.get_known_types(), parser.get_errors() };
    }

    static std::pair<std::list<stmnt_uptr>, std::vector<compiler::parsing_error>> get_statements(std::vector<compiler::token> &tokens) {
        if (tokens.back().m_type == compiler::token_type::_EOF) {
            tokens.pop_back();
        }

        std::vector<compiler::token> function_def_tokens {
            compiler::token(compiler::token_type::IDENTIFIER, "i32", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "main", 0, 1),
            compiler::token(compiler::token_type::LEFT_PAREN, "(", 0, 1),
            compiler::token(compiler::token_type::RIGHT_PAREN, ")", 0, 1),
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 1),
        };

        function_def_tokens.insert(function_def_tokens.end(), tokens.begin(), tokens.end());


        function_def_tokens.push_back(compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0, 1));
        function_def_tokens.push_back(compiler::token(compiler::token_type::_EOF, "", 0, 1));

        compiler::Parser parser{function_def_tokens};

        auto [functions, _, errors] = get_parse_results(function_def_tokens);

        return { !functions.empty() ? std::move(static_cast<ast::function_definition*>(functions[0].get())->m_body.m_statements) : std::list<stmnt_uptr>{}, errors };
    } 

    TEST(COMPILER, LexerEmpty) {
        const std::string empty = "";
        const auto [tokens, errors] = get_tokens(empty);
        const compiler::token eof = compiler::token(compiler::token_type::_EOF, "", 0, 1);
        
        EXPECT_EQ(tokens.back(), eof);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, WrongStatementCast) {
        std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "a", 0, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 1),
            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);

        const ast::variable_declaration* actual = dynamic_cast<const ast::variable_declaration*>(statements.front().get());
        EXPECT_EQ(actual, nullptr);
    }

    TEST(COMPILER, WrongExpressionCast) {
        std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "a", 0, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 1),
            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
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

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::PLUS, "+", 0, 1),
            compiler::token(compiler::token_type::DOT, ".", 0, 1),
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 1),
            compiler::token(compiler::token_type::RIGHT_PAREN, ")", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerDoubleChars) {
        const std::string chars = "!=,==}<==";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::BANG_EQUAL, "!=", 0, 1),
            compiler::token(compiler::token_type::COMMA, ",", 0, 1),
            compiler::token(compiler::token_type::EQUAL_EQUAL, "==", 0, 1),
            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0, 1),
            compiler::token(compiler::token_type::LESS_EQUAL, "<=", 0, 1),
            compiler::token(compiler::token_type::EQUAL, "=", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerInvalidCharacter) {
        const std::string chars = "@";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::_EOF, "", 0, 1),
        };

        const std::vector<compiler::lexing_error> expected_errors = {
            compiler::lexing_error(1, "invalid token '@'")
        };

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors, expected_errors);
    }

    TEST(COMPILER, LexerComment) {
        const std::string chars = "/=.//daaindasd}\n+";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::SLASH_EQUAL, "/=", 0, 1),
            compiler::token(compiler::token_type::DOT, ".", 0, 1),
            compiler::token(compiler::token_type::PLUS, "+", 0, 2),
            compiler::token(compiler::token_type::_EOF, "", 0, 2),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerString) {
        const std::string chars = "++\"blackeyeGalaxy_abc\"++";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::PLUS_PLUS, "++", 0, 1),
            compiler::token(compiler::token_type::STRING, "\"blackeyeGalaxy_abc\"", "blackeyeGalaxy_abc", 1),
            compiler::token(compiler::token_type::PLUS_PLUS, "++", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1),
        };

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerStringEOF) {
        const std::string chars = "\"blackeyeGalaxy_abc";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::_EOF, "", 0, 1),
        };

        const std::vector<compiler::lexing_error> expected_errors = {
            compiler::lexing_error(1, "unterminated string literal")
        };

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(expected_errors, errors);
    }

    TEST(COMPILER, LexerSimpleInt) {
        const std::string chars = "]{123\n34";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::RIGHT_SQUARE, "]", 0, 1),
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 1),
            compiler::token(compiler::token_type::INT, "123", 123, 1),
            compiler::token(compiler::token_type::INT, "34", 34, 2),
            compiler::token(compiler::token_type::_EOF, "", 0, 2),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerSimpleDouble) {
        const std::string chars = "123.45\"a\"";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::DOUBLE, "123.45", 123.45, 1),
            compiler::token(compiler::token_type::STRING, "\"a\"", "a", 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerSimpleHex) {
        const std::string chars = "0x123F.abc";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::HEX, "0x123F", 0x123F, 1),
            compiler::token(compiler::token_type::DOT, ".", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "abc", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1),
        };
        
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerSimpleSID) {
        const std::string chars = "1.3-=#ellie";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::DOUBLE, "1.3", 1.3, 1),
            compiler::token(compiler::token_type::MINUS_EQUAL, "-=", 0, 1),
            compiler::token(compiler::token_type::SID, "#ellie", "ellie", 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }


    TEST(COMPILER, LexerSimpleIdentifier) {
        const std::string chars = "abc\ndef;";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::IDENTIFIER, "abc", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "def", 0, 2),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 2),
            compiler::token(compiler::token_type::_EOF, "", 0, 2),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerSimpleKeywords) {
        const std::string chars = "return;";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::RETURN, "return", 0, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1),
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
        
        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::IDENTIFIER, "int", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "main", 0, 1),
            compiler::token(compiler::token_type::LEFT_PAREN, "(", 0, 1),
            compiler::token(compiler::token_type::RIGHT_PAREN, ")", 0, 1),
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 2),
            compiler::token(compiler::token_type::IDENTIFIER, "int", 0, 3),
            compiler::token(compiler::token_type::IDENTIFIER, "a", 0, 3),
            compiler::token(compiler::token_type::EQUAL, "=", 0, 3),
            compiler::token(compiler::token_type::INT, "0", 0, 3),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 3),
            compiler::token(compiler::token_type::IDENTIFIER, "a", 0, 4),
            compiler::token(compiler::token_type::PLUS_EQUAL, "+=", 0, 4),
            compiler::token(compiler::token_type::INT, "1", 0, 4),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 4),
            compiler::token(compiler::token_type::RETURN, "return", 0, 5),
            compiler::token(compiler::token_type::IDENTIFIER, "a", 0, 5),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 5),
            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0, 6),
            compiler::token(compiler::token_type::_EOF, "", 0, 6),
        };
    }

    TEST(COMPILER, LexerComplexSids) {
        const std::string chars = "#simple_sid #%alloc-array? #=f(test123)";
        const auto [tokens, errors] = get_tokens(chars);
        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::SID, "#simple_sid", "simple_sid", 1),
            compiler::token(compiler::token_type::SID, "#%alloc-array?", "%alloc-array?", 1),
            compiler::token(compiler::token_type::SID, "#=f", "=f", 1),
            compiler::token(compiler::token_type::LEFT_PAREN, "(", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "test123", 0, 1),
            compiler::token(compiler::token_type::RIGHT_PAREN, ")", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1),
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

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::STRUCT, "struct", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "Person", 0, 1),
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 1),

            compiler::token(compiler::token_type::IDENTIFIER, "sid", 0, 2),
            compiler::token(compiler::token_type::IDENTIFIER, "name", 0, 2),
            compiler::token(compiler::token_type::EQUAL, "=", 0, 2),
            compiler::token(compiler::token_type::SID, "#ellie", "ellie", 2),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 2),

            compiler::token(compiler::token_type::IDENTIFIER, "double", 0, 3),
            compiler::token(compiler::token_type::IDENTIFIER, "damage", 0, 3),
            compiler::token(compiler::token_type::EQUAL, "=", 0, 3),
            compiler::token(compiler::token_type::DOUBLE, "5.9", 5.9, 3),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 3),

            compiler::token(compiler::token_type::IDENTIFIER, "int", 0, 4),
            compiler::token(compiler::token_type::IDENTIFIER, "hexVal", 0, 4),
            compiler::token(compiler::token_type::EQUAL, "=", 0, 4),
            compiler::token(compiler::token_type::HEX, "0x1A3F", 0x1A3F, 4),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 4),

            compiler::token(compiler::token_type::IDENTIFIER, "string", 0, 5),
            compiler::token(compiler::token_type::IDENTIFIER, "msg", 0, 5),
            compiler::token(compiler::token_type::EQUAL, "=", 0, 5),
            compiler::token(compiler::token_type::STRING, "\"Hello, world!\"", "Hello, world!", 5),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 5),

            compiler::token(compiler::token_type::IF, "if", 0, 6),
            compiler::token(compiler::token_type::LEFT_PAREN, "(", 0, 6),
            compiler::token(compiler::token_type::IDENTIFIER, "damage", 0, 6),
            compiler::token(compiler::token_type::GREATER, ">", 0, 6),
            compiler::token(compiler::token_type::DOUBLE, "6.0", 6.0, 6),
            compiler::token(compiler::token_type::RIGHT_PAREN, ")", 0, 6),
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 6),

            compiler::token(compiler::token_type::IDENTIFIER, "msg", 0, 7),
            compiler::token(compiler::token_type::EQUAL, "=", 0, 7),
            compiler::token(compiler::token_type::STRING, "\"Joel Miller\"", "Joel Miller", 7),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 7),

            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0, 8),
            compiler::token(compiler::token_type::ELSE, "else", 0, 8),
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 8),

            compiler::token(compiler::token_type::IDENTIFIER, "msg", 0, 9),
            compiler::token(compiler::token_type::EQUAL, "=", 0, 9),
            compiler::token(compiler::token_type::STRING, "\"Test String\"", "Test String", 9),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 9),

            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0, 10),

            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0, 11),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 11),
            compiler::token(compiler::token_type::_EOF, "", 0, 11),
        };

        const auto [tokens, errors] = get_tokens(chars);
        for (u32 i = 0; i < tokens.size(); ++i) {
            ASSERT_EQ(tokens[i], expected[i]);
        }
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, SimpleNumParse1) {
        std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::INT, "1", 1, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 1, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1) 
        };

        const auto [statements, errors] = get_statements(tokens);
        
        const ast::literal expected = ast::literal(1);

        const ast::expression& actual = *dynamic_cast<const ast::expression_stmt*>(statements.front().get())->m_expression;

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, SimpleNumParse2) {
        std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::INT, "1", 1, 1),
            compiler::token(compiler::token_type::PLUS, "+", 0, 1),
            compiler::token(compiler::token_type::INT, "2", 2, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 2, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const ast::expression& actual = *dynamic_cast<const ast::expression_stmt*>(statements.front().get())->m_expression;
        
        const ast::add_expr expected{
            compiler::token(compiler::token_type::PLUS, "+", 0, 1),
            std::make_unique<ast::literal>(1),
            std::make_unique<ast::literal>(2)
        };

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, SimpleNumParse3) {
        std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::INT, "1", 1, 1),
            compiler::token(compiler::token_type::PLUS, "+", 0, 1),
            compiler::token(compiler::token_type::INT, "2", 2, 1),
            compiler::token(compiler::token_type::STAR, "*", 0, 1),
            compiler::token(compiler::token_type::INT, "5", 5, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 5, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const ast::expression& actual = *dynamic_cast<const ast::expression_stmt*>(statements.front().get())->m_expression;
        
        expr_uptr left = std::make_unique<ast::mul_expr>(
            compiler::token(compiler::token_type::STAR, "*", 0, 1),
            std::make_unique<ast::literal>(2),
            std::make_unique<ast::literal>(5)
        );

        const ast::add_expr expected{
            compiler::token(compiler::token_type::PLUS, "+", 0, 1),
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
        std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::LEFT_PAREN, "(", 1, 1),
            compiler::token(compiler::token_type::INT, "1", 1, 1),
            compiler::token(compiler::token_type::PLUS, "+", 0, 1),
            compiler::token(compiler::token_type::INT, "2", 2, 1),
            compiler::token(compiler::token_type::RIGHT_PAREN, ")", 1, 1),
            compiler::token(compiler::token_type::STAR, "*", 0, 1),
            compiler::token(compiler::token_type::INT, "5", 5, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 5, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const ast::expression& actual = *dynamic_cast<const ast::expression_stmt*>(statements.front().get())->m_expression;
        
        auto left = std::make_unique<ast::grouping>(std::make_unique<ast::add_expr>(
            compiler::token(compiler::token_type::PLUS, "+", 0, 1),
            std::make_unique<ast::literal>(1),
            std::make_unique<ast::literal>(2)
        ));

        const ast::mul_expr expected{
            compiler::token(compiler::token_type::STAR, "*", 0, 1),
            std::move(left),
            std::make_unique<ast::literal>(5)
        };

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, VariableDeclaration0) {
        std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::IDENTIFIER, "u16", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "number", 0, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const auto& actual = *static_cast<const ast::variable_declaration*>(statements.front().get());

        const ast::variable_declaration expected{ast::make_type_from_prim(ast::primitive_kind::U16), "number"};

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, VariableDeclaration1) {
        std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::IDENTIFIER, "u16", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "number", 0, 1),
            compiler::token(compiler::token_type::EQUAL, "=", 0, 1),
            compiler::token(compiler::token_type::INT, "2", 2, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const auto& actual = *static_cast<const ast::variable_declaration*>(statements.front().get());

        const ast::variable_declaration expected{ast::make_type_from_prim(ast::primitive_kind::U16), "number", 2};

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, SimpleBlock) {
        std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "a", 0, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 1),
            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const ast::block& actual = *dynamic_cast<const ast::block*>(statements.front().get());

        std::list<stmnt_uptr> expected_statements{};
        expected_statements.push_back(std::move(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::identifier>(compiler::token(compiler::token_type::IDENTIFIER, "a", 0, 1))
        )));

        const ast::block expected{std::move(expected_statements)};

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, SimpleBlockError) {
        std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "b", 0, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 1),
            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const ast::block& actual = *dynamic_cast<const ast::block*>(statements.front().get());

        std::list<stmnt_uptr> expected_statements{};
        expected_statements.push_back(std::move(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::identifier>(compiler::token(compiler::token_type::IDENTIFIER, "a", 0, 1))
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
                    compiler::token(compiler::token_type::PLUS, "+", 0, 1),
                    std::make_unique<ast::identifier>("x"),
                    std::make_unique<ast::identifier>("y")
                )
            )
        ));
        expected.push_back(std::make_unique<ast::variable_declaration>(ast::make_type_from_prim(ast::primitive_kind::U32), "x", 0));
        expected.push_back(std::make_unique<ast::variable_declaration>(ast::make_type_from_prim(ast::primitive_kind::U32), "y", 1));
        expected.push_back(std::make_unique<ast::while_stmt>(
            std::make_unique<ast::compare_expr>(
                compiler::token(compiler::token_type::LESS, "<", 0, 1),
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
                compiler::token(compiler::token_type::MINUS, "-", 0, 1),
                std::make_unique<ast::add_expr>(
                    compiler::token(compiler::token_type::PLUS, "+", 0, 1),
                    std::make_unique<ast::literal>(1),
                    std::make_unique<ast::mul_expr>(
                        compiler::token(compiler::token_type::STAR, "*", 0, 1),
                        std::make_unique<ast::literal>(2),
                        std::make_unique<ast::literal>(3)
                    )
                ),
                std::make_unique<ast::div_expr>(
                    compiler::token(compiler::token_type::SLASH, "/", 0, 1),
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
                    compiler::token(compiler::token_type::PLUS, "+", 0, 1),
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
            compiler::token(compiler::token_type::PLUS, "+", 0, 1),
            std::make_unique<ast::literal>(2),
            std::make_unique<ast::literal>(3)
        ));

        std::list<stmnt_uptr> expected;
        expected.push_back(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::call_expr>(
                compiler::token(compiler::token_type::IDENTIFIER, "doSomething", 0, 1),
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
                    compiler::token(compiler::token_type::SLASH, "/", 0, 1),
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

        dconstruct::compiler::scope type_env{};
        const auto errors = statements.front()->check_semantics(type_env);

        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexError1) {
        const std::string chars = "@";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::_EOF, "", 0, 1),
        };

        const std::vector<compiler::lexing_error> expected_errors = {
            compiler::lexing_error(1, "invalid token '@'")
        };

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(expected_errors, errors);
    }

    TEST(COMPILER, ParseError1) {
        std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::INT, "1", 1, 1),
            compiler::token(compiler::token_type::PLUS, "+", 0, 1),
            compiler::token(compiler::token_type::_EOF, ";", 0, 1)
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
        const auto [functions, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(functions.size(), 0);
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
        const auto [functions, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(functions.size(), 0);
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
        const auto [functions, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(functions.size(), 1);
        
        compiler::scope scope{};
        scope.n_namesToTypes = types;
        const auto semantic_errors = functions[0]->check_semantics(scope);

        std::vector<ast::semantic_check_error> empty{};
        EXPECT_EQ(semantic_errors, empty);
    }

    TEST(COMPILER, Declaration1) {
        const std::string code = "i32 main () { i32 x = 0; i32 y = 0; return x = y; }";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [functions, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(functions.size(), 1);
        
        compiler::scope scope{};
        scope.n_namesToTypes = types;
        const auto semantic_errors = functions[0]->check_semantics(scope);

        std::vector<ast::semantic_check_error> empty{};
        EXPECT_EQ(semantic_errors, empty);
    }

    TEST(COMPILER, Using1) {
        const std::string code = 
        "using #display as (string, i32) -> void;"
        "using #5445173390656D6D as (string, i32, i32) -> string sprintf;"
        "i32 main() {"
        "    string message = sprintf(\"Hello World from DC version %d.%d\", 0, 0);"
        "    display(message, 19);"
        "    return 0;"
        "}";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [functions, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(functions.size(), 3);

        compiler::scope scope{};
        scope.n_namesToTypes = types;
        std::vector<ast::semantic_check_error> semantic_errors;
        for (const auto& decl : functions) {
            const auto new_errors = decl->check_semantics(scope);
            semantic_errors.insert(semantic_errors.end(), new_errors.begin(), new_errors.end());
        }
        ASSERT_EQ(scope.m_sidAliases["sprintf"].first, 0x5445173390656D6D);

        std::vector<ast::semantic_check_error> empty{};
        EXPECT_EQ(semantic_errors, empty);
    }
}
