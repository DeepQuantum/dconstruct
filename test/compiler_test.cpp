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

    static std::pair<std::vector<stmnt_uptr>, std::vector<compiler::parsing_error>> get_statements(const std::vector<compiler::token> &tokens) {
        compiler::Parser parser{tokens};
        return { parser.parse(), parser.get_errors() };
    }

    TEST(COMPILER, LexerEmpty) {
        const std::string empty = "";
        const auto [tokens, errors] = get_tokens(empty);
        const compiler::token eof = compiler::token(compiler::token_type::_EOF, "", 0, 1);
        
        EXPECT_EQ(tokens.back(), eof);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, WrongStatementCast) {
        const std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "a", 0, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 1),
            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);

        const ast::variable_declaration* actual = dynamic_cast<const ast::variable_declaration*>(statements[0].get());
        EXPECT_EQ(actual, nullptr);
    }

    TEST(COMPILER, WrongExpressionCast) {
        const std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "a", 0, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 1),
            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        const ast::block* block = static_cast<const ast::block*>(statements[0].get());
        const ast::expression_stmt* expr = static_cast<const ast::expression_stmt*>(block->m_statements[0].get());
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
            compiler::token(compiler::token_type::INT, "int", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "main", 0, 1),
            compiler::token(compiler::token_type::LEFT_PAREN, "(", 0, 1),
            compiler::token(compiler::token_type::RIGHT_PAREN, ")", 0, 1),
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 2),
            compiler::token(compiler::token_type::INT, "int", 0, 3),
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

    TEST(COMPILER, LexerProgramWithExtendedFeatures) {
        const std::string chars = 
            "struct Person {\n"
            "\tSID name = #john_doe;\n"
            "\tdouble height = 5.9;\n"
            "\tint hexVal = 0x1A3F;\n"
            "\tstring msg = \"Hello, world!\";\n"
            "\tif (height > 6.0) {\n"
            "\t\tmsg = \"Tall person\";\n"
            "\t} else {\n"
            "\t\tmsg = \"Not so tall\";\n"
            "\t}\n"
            "};"
            ;

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::STRUCT, "struct", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "Person", 0, 1),
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 1),

            compiler::token(compiler::token_type::IDENTIFIER, "SID", 0, 2),
            compiler::token(compiler::token_type::IDENTIFIER, "name", 0, 2),
            compiler::token(compiler::token_type::EQUAL, "=", 0, 2),
            compiler::token(compiler::token_type::SID, "#john_doe", "john_doe", 2),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 2),

            compiler::token(compiler::token_type::IDENTIFIER, "double", 0, 3),
            compiler::token(compiler::token_type::IDENTIFIER, "height", 0, 3),
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
            compiler::token(compiler::token_type::IDENTIFIER, "height", 0, 6),
            compiler::token(compiler::token_type::GREATER, ">", 0, 6),
            compiler::token(compiler::token_type::DOUBLE, "6.0", 6.0, 6),
            compiler::token(compiler::token_type::RIGHT_PAREN, ")", 0, 6),
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 6),

            compiler::token(compiler::token_type::IDENTIFIER, "msg", 0, 7),
            compiler::token(compiler::token_type::EQUAL, "=", 0, 7),
            compiler::token(compiler::token_type::STRING, "\"Tall person\"", "Tall person", 7),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 7),

            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0, 8),
            compiler::token(compiler::token_type::ELSE, "else", 0, 8),
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 8),

            compiler::token(compiler::token_type::IDENTIFIER, "msg", 0, 9),
            compiler::token(compiler::token_type::EQUAL, "=", 0, 9),
            compiler::token(compiler::token_type::STRING, "\"Not so tall\"", "Not so tall", 9),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 9),

            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0, 10),

            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0, 11),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 11),
            compiler::token(compiler::token_type::_EOF, "", 0, 11),
        };

        const auto [tokens, errors] = get_tokens(chars);
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, SimpleNumParse1) {
        const std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::INT, "1", 1, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 1, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1) 
        };

        const auto [statements, errors] = get_statements(tokens);
        
        const ast::literal expected = ast::literal(1);

        const ast::expression& actual = *dynamic_cast<const ast::expression_stmt*>(statements[0].get())->m_expression;

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, SimpleNumParse2) {
        const std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::INT, "1", 1, 1),
            compiler::token(compiler::token_type::PLUS, "+", 0, 1),
            compiler::token(compiler::token_type::INT, "2", 2, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 2, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const ast::expression& actual = *dynamic_cast<const ast::expression_stmt*>(statements[0].get())->m_expression;
        
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
        const std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::INT, "1", 1, 1),
            compiler::token(compiler::token_type::PLUS, "+", 0, 1),
            compiler::token(compiler::token_type::INT, "2", 2, 1),
            compiler::token(compiler::token_type::STAR, "*", 0, 1),
            compiler::token(compiler::token_type::INT, "5", 5, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 5, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const ast::expression& actual = *dynamic_cast<const ast::expression_stmt*>(statements[0].get())->m_expression;
        
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
        const std::vector<compiler::token> tokens = {
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
        const std::vector<compiler::token> tokens = {
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

        const ast::expression& actual = *dynamic_cast<const ast::expression_stmt*>(statements[0].get())->m_expression;
        
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
        const std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::IDENTIFIER, "u16", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "number", 0, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const auto& actual = *static_cast<const ast::variable_declaration*>(statements[0].get());

        const ast::variable_declaration expected{"u16", "number"};

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, VariableDeclaration1) {
        const std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::IDENTIFIER, "u16", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "number", 0, 1),
            compiler::token(compiler::token_type::EQUAL, "=", 0, 1),
            compiler::token(compiler::token_type::INT, "2", 2, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const auto& actual = *static_cast<const ast::variable_declaration*>(statements[0].get());

        const ast::variable_declaration expected{"u16", "number", 2};

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, SimpleBlock) {
        const std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "a", 0, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 1),
            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const ast::block& actual = *dynamic_cast<const ast::block*>(statements[0].get());

        std::vector<stmnt_uptr> expected_statements{};
        expected_statements.push_back(std::move(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::identifier>(compiler::token(compiler::token_type::IDENTIFIER, "a", 0, 1))
        )));

        const ast::block expected{std::move(expected_statements)};

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, SimpleBlockError) {
        const std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "b", 0, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0, 1),
            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const ast::block& actual = *dynamic_cast<const ast::block*>(statements[0].get());

        std::vector<stmnt_uptr> expected_statements{};
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

        const auto [tokens, lex_errors] = get_tokens(code);

        const auto [statements, parse_errors] = get_statements(tokens);

        EXPECT_EQ(statements.size(), 3);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);

        std::vector<stmnt_uptr> expected;

        std::vector<stmnt_uptr> block_stmnt;

        block_stmnt.push_back(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::assign_expr>(
                compiler::token(compiler::token_type::IDENTIFIER, "x", 0, 1),
                std::make_unique<ast::add_expr>(
                    compiler::token(compiler::token_type::PLUS, "+", 0, 1),
                    std::make_unique<ast::identifier>("x"),
                    std::make_unique<ast::identifier>("y")
                )
            )
        ));

        expected.push_back(std::make_unique<ast::variable_declaration>("i32", "x", 0));
        expected.push_back(std::make_unique<ast::variable_declaration>("i32", "y", 1));
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
}
