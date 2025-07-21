#include <gtest/gtest.h>
#include "lexer.h"

namespace dconstruct::testing {
    static std::pair<std::vector<compiler::token>, std::vector<compiler::lexing_error>> get_tokens(const std::string &string) {
        dconstruct::compiler::Lexer lexer = dconstruct::compiler::Lexer(string);
        return { lexer.scan_tokens(), lexer.get_errors() };
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
            compiler::lexing_error(1, "invalid token '@'")
        };

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors, expected_errors);
    }

    TEST(COMPILER, LexerComment) {
        const std::string chars = "/=.//daaindasd}\n+";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::SLASH_EQUAL, "/=", 0ULL, 1),
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
            compiler::token(compiler::token_type::PLUS_PLUS, "++", 0ULL, 1),
            compiler::token(compiler::token_type::STRING, "\"blackeyeGalaxy_abc\"", "blackeyeGalaxy_abc", 1),
            compiler::token(compiler::token_type::PLUS_PLUS, "++", 0ULL, 1),
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
        const std::string chars = "0x123F.abc";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::HEX, "0x123F", 0x123FULL, 1),
            compiler::token(compiler::token_type::DOT, ".", 0ULL, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "abc", 0ULL, 1),
            compiler::token(compiler::token_type::_EOF, "", 0ULL, 1),
        };
        
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerSimpleSID) {
        const std::string chars = "1.3-=#ellie";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::DOUBLE, "1.3", 1.3, 1),
            compiler::token(compiler::token_type::MINUS_EQUAL, "-=", 0ULL, 1),
            compiler::token(compiler::token_type::SID, "#ellie", "ellie", 1),
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

    TEST(COMPILER, LexerSimpleKeywords) {
        const std::string chars = "return;";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compiler::token> expected = {
            compiler::token(compiler::token_type::RETURN, "return", 0ULL, 1),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0ULL, 1),
            compiler::token(compiler::token_type::_EOF, "", 0ULL, 1),
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
            compiler::token(compiler::token_type::INT, "int", 0ULL, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "main", 0ULL, 1),
            compiler::token(compiler::token_type::LEFT_PAREN, "(", 0ULL, 1),
            compiler::token(compiler::token_type::RIGHT_PAREN, ")", 0ULL, 1),
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0ULL, 2),
            compiler::token(compiler::token_type::INT, "int", 0ULL, 3),
            compiler::token(compiler::token_type::IDENTIFIER, "a", 0ULL, 3),
            compiler::token(compiler::token_type::EQUAL, "=", 0ULL, 3),
            compiler::token(compiler::token_type::INT, "0", 0ULL, 3),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0ULL, 3),
            compiler::token(compiler::token_type::IDENTIFIER, "a", 0ULL, 4),
            compiler::token(compiler::token_type::PLUS_EQUAL, "+=", 0ULL, 4),
            compiler::token(compiler::token_type::INT, "1", 0ULL, 4),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0ULL, 4),
            compiler::token(compiler::token_type::RETURN, "return", 0ULL, 5),
            compiler::token(compiler::token_type::IDENTIFIER, "a", 0ULL, 5),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0ULL, 5),
            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0ULL, 6),
            compiler::token(compiler::token_type::_EOF, "", 0ULL, 6),
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
            compiler::token(compiler::token_type::STRUCT, "struct", 0ULL, 1),
            compiler::token(compiler::token_type::IDENTIFIER, "Person", 0ULL, 1),
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0ULL, 1),

            compiler::token(compiler::token_type::IDENTIFIER, "SID", 0ULL, 2),
            compiler::token(compiler::token_type::IDENTIFIER, "name", 0ULL, 2),
            compiler::token(compiler::token_type::EQUAL, "=", 0ULL, 2),
            compiler::token(compiler::token_type::SID, "#john_doe", "john_doe", 2),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0ULL, 2),

            compiler::token(compiler::token_type::IDENTIFIER, "double", 0ULL, 3),
            compiler::token(compiler::token_type::IDENTIFIER, "height", 0ULL, 3),
            compiler::token(compiler::token_type::EQUAL, "=", 0ULL, 3),
            compiler::token(compiler::token_type::DOUBLE, "5.9", 5.9, 3),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0ULL, 3),

            compiler::token(compiler::token_type::IDENTIFIER, "int", 0ULL, 4),
            compiler::token(compiler::token_type::IDENTIFIER, "hexVal", 0ULL, 4),
            compiler::token(compiler::token_type::EQUAL, "=", 0ULL, 4),
            compiler::token(compiler::token_type::HEX, "0x1A3F", 0x1A3FULL, 4),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0ULL, 4),

            compiler::token(compiler::token_type::IDENTIFIER, "string", 0ULL, 5),
            compiler::token(compiler::token_type::IDENTIFIER, "msg", 0ULL, 5),
            compiler::token(compiler::token_type::EQUAL, "=", 0ULL, 5),
            compiler::token(compiler::token_type::STRING, "\"Hello, world!\"", "Hello, world!", 5),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0ULL, 5),

            compiler::token(compiler::token_type::IF, "if", 0ULL, 6),
            compiler::token(compiler::token_type::LEFT_PAREN, "(", 0ULL, 6),
            compiler::token(compiler::token_type::IDENTIFIER, "height", 0ULL, 6),
            compiler::token(compiler::token_type::GREATER, ">", 0ULL, 6),
            compiler::token(compiler::token_type::DOUBLE, "6.0", 6.0, 6),
            compiler::token(compiler::token_type::RIGHT_PAREN, ")", 0ULL, 6),
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0ULL, 6),

            compiler::token(compiler::token_type::IDENTIFIER, "msg", 0ULL, 7),
            compiler::token(compiler::token_type::EQUAL, "=", 0ULL, 7),
            compiler::token(compiler::token_type::STRING, "\"Tall person\"", "Tall person", 7),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0ULL, 7),

            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0ULL, 8),
            compiler::token(compiler::token_type::ELSE, "else", 0ULL, 8),
            compiler::token(compiler::token_type::LEFT_BRACE, "{", 0ULL, 8),

            compiler::token(compiler::token_type::IDENTIFIER, "msg", 0ULL, 9),
            compiler::token(compiler::token_type::EQUAL, "=", 0ULL, 9),
            compiler::token(compiler::token_type::STRING, "\"Not so tall\"", "Not so tall", 9),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0ULL, 9),

            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0ULL, 10),

            compiler::token(compiler::token_type::RIGHT_BRACE, "}", 0ULL, 11),
            compiler::token(compiler::token_type::SEMICOLON, ";", 0ULL, 11),
            compiler::token(compiler::token_type::_EOF, "", 0ULL, 11),
        };

        const auto [tokens, errors] = get_tokens(chars);
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }
}
