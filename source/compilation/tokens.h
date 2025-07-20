#include "base.h"
#include <variant>

namespace dconstruct::compiler {
    enum token_type {
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

        IDENTIFIER, STRING, INT, DOUBLE, HEX, SID,

        ELSE, FALSE, IF, _NULL, RETURN, STRUCT, TRUE, WHILE, 
        
        _EOF, EMPTY
    };

    struct token {
        using t_literal = std::variant<std::string, u64, f64>;

        token(const token_type type, const std::string &lexeme, const t_literal &literal = 0ULL, const u32 line = INT_MAX) :
        m_type(type),
        m_lexeme(lexeme),
        m_literal(literal),
        m_line(line) {}

        [[nodiscard]] b8 operator==(const token &rhs) const {
            const b8 result = m_type == rhs.m_type && m_lexeme == rhs.m_lexeme && m_literal == rhs.m_literal && m_line == rhs.m_line;
            return result;
        }

        token_type m_type;
        std::string m_lexeme;
        t_literal m_literal;
        u32 m_line;
    };
}