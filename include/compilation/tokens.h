#pragma once

#include "base.h"
#include "ast/type.h"
#include <variant>

namespace dconstruct::compiler {
    enum class token_type {
        LEFT_PAREN, RIGHT_PAREN, 
        LEFT_BRACE, RIGHT_BRACE, 
        LEFT_SQUARE, RIGHT_SQUARE,
        COMMA, DOT, SEMICOLON,

        PLUS, PLUS_EQUAL, PLUS_PLUS,
        MINUS, MINUS_EQUAL, MINUS_MINUS,
        STAR, STAR_EQUAL,
        SLASH, SLASH_EQUAL,
        BANG, BANG_EQUAL,
        EQUAL, EQUAL_EQUAL,
        GREATER, GREATER_EQUAL,
        LESS, LESS_EQUAL,
        PIPE, PIPE_PIPE,
        CARET, CARET_CARET,
        AMPERSAND, AMPERSAND_AMPERSAND,

        AND, OR,

        IDENTIFIER, 
        
        STRING, INT, DOUBLE, HEX, SID, _NULL,

        ELSE, FALSE, IF, RETURN, STRUCT, TRUE, WHILE, FOR,
        
        _EOF, EMPTY
    };

    struct token {
        token(token_type type, std::string lexeme,
            ast::primitive_value literal = 0,
            u32 line = INT_MAX) noexcept
            :m_literal(std::move(literal)),
            m_lexeme(std::move(lexeme)),
            m_type(type),
            m_line(line) {}

        [[nodiscard]] inline b8 operator==(const token &rhs) const {
            const b8 result = m_type == rhs.m_type && m_lexeme == rhs.m_lexeme && m_literal == rhs.m_literal && m_line == rhs.m_line;
            return result;
        }

        ast::primitive_value m_literal;
        std::string m_lexeme;
        token_type m_type;
        u32 m_line;
    };
}