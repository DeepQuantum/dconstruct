#include <gtest/gtest.h>
#include "lexer.h"

namespace dconstruct::testing {
    std::pair<std::vector<compiler::token>, std::vector<compiler::lexing_error>> get_tokens(const std::string &string) {
        dconstruct::compiler::Lexer lexer = dconstruct::compiler::Lexer(string);
        return { lexer.scan_tokens(), lexer.get_errors() };
    } 

    void print_tokens(const std::vector<compiler::token> &tokens) {
        for (const auto& token : tokens) {
            std::string literal;
            switch(token.m_type) {
                case compiler::token_type::STRING: literal = std::get<std::string>(token.m_literal); break;
                case compiler::token_type::INT:
                case compiler::token_type::HEX: literal = std::to_string(std::get<u64>(token.m_literal)); break;
                case compiler::token_type::DOUBLE: literal = std::to_string(std::get<f64>(token.m_literal)); break;
            }
            std::cout 
                << "type: " << token.m_type
                << " lexeme: " << token.m_lexeme
                << " literal: " << literal
                << " line: " << token.m_line
                << '\n';
        }
    }

    TEST(COMPILER, LexerEmpty) {
        const std::string empty = "";
        const auto [tokens, errors] = get_tokens(empty);
        const compiler::token eof = compiler::token(compiler::token_type::_EOF, "", 0ULL, 1);
        
        EXPECT_EQ(tokens.back(), eof);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerBasicChars) {
        const std::string chars = "+.{)";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::PLUS, "+", 0ULL, 1),
            compiler::token(compiler::token_type::DOT, ".", 0ULL, 1),
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0ULL, 1),
            compiler::token(compiler::token_type::RIGHT_PAREN, ")", 0ULL, 1),
            compiler::token(compiler::token_type::_EOF, "", 0ULL, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerDoubleChars) {
        const std::string chars = "!=,==}<==";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::BANG_EQUAL, "!=", 0ULL, 1),
            compiler::token(compiler::token_type::COMMA, ",", 0ULL, 1),
            compiler::token(compiler::token_type::EQUAL_EQUAL, "==", 0ULL, 1),
            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0ULL, 1),
            compiler::token(compiler::token_type::LESS_EQUAL, "<=", 0ULL, 1),
            compiler::token(compiler::token_type::EQUAL, "=", 0ULL, 1),
            compiler::token(compiler::token_type::_EOF, "", 0ULL, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerInvalidCharacter) {
        const std::string chars = "@";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::_EOF, "", 0ULL, 1),
        };

        const std::vector<compiler::lexing_error> expected_errors = {
            compiler::lexing_error(1, "invalid token @")
        };

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors, expected_errors);
    }

    TEST(COMPILER, LexerComment) {
        const std::string chars = "/=.//daaindasd}\n+";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::SLASH, "/", 0ULL, 1),
            compiler::token(compiler::token_type::EQUAL, "=", 0ULL, 1),
            compiler::token(compiler::token_type::DOT, ".", 0ULL, 1),
            compiler::token(compiler::token_type::PLUS, "+", 0ULL, 2),
            compiler::token(compiler::token_type::_EOF, "", 0ULL, 2),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerString) {
        const std::string chars = "++\"blackeyeGalaxy_abc\"++";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::PLUS, "+", 0ULL, 1),
            compiler::token(compiler::token_type::PLUS, "+", 0ULL, 1),
            compiler::token(compiler::token_type::STRING, "\"blackeyeGalaxy_abc\"", "blackeyeGalaxy_abc", 1),
            compiler::token(compiler::token_type::PLUS, "+", 0ULL, 1),
            compiler::token(compiler::token_type::PLUS, "+", 0ULL, 1),
            compiler::token(compiler::token_type::_EOF, "", 0ULL, 1),
        };

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerStringEOF) {
        const std::string chars = "\"blackeyeGalaxy_abc";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::_EOF, "", 0ULL, 1),
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
            compiler::token(compiler::token_type::RIGHT_SQUARE, "]", 0ULL, 1),
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0ULL, 1),
            compiler::token(compiler::token_type::INT, "123", 123ULL, 1),
            compiler::token(compiler::token_type::INT, "34", 34ULL, 2),
            compiler::token(compiler::token_type::_EOF, "", 0ULL, 2),
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
            compiler::token(compiler::token_type::_EOF, "", 0ULL, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerSimpleHex) {
        const std::string chars = "0x123.abc";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::HEX, "0x123", 0x123ULL, 1),
            compiler::token(compiler::token_type::DOT, ".", 0ULL, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "abc", 0ULL, 1),
            compiler::token(compiler::token_type::_EOF, "", 0ULL, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }


    TEST(COMPILER, LexerSimpleIdentifier) {
        const std::string chars = "abc\ndef;";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::IDENTIFIER, "abc", 0ULL, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "def", 0ULL, 2),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0ULL, 2),
            compiler::token(compiler::token_type::_EOF, "", 0ULL, 2),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }
}
