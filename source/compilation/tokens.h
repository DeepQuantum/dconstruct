#include "base.h"
#include <variant>

namespace dconstruct::compiler {
    enum token_type {
        LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
        COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

        BANG, BANG_EQUAL,
        EQUAL, EQUAL_EQUAL,
        GREATER, GREATER_EQUAL,
        LESS, LESS_EQUAL,

        IDENTIFIER, STRING, NUMBER, SID,

        ELSE, FALSE, IF, _NULL, RETURN, TRUE, WHILE,
        
        _EOF, EMPTY
    };

    struct token {
        using t_literal = std::variant<std::string, u64>;

        token(const token_type type, const std::string &lexeme, const std::variant<std::string, u64> &literal = 0ULL, const u32 line = INT_MAX) :
        m_type(type),
        m_lexeme(lexeme),
        m_literal(literal),
        m_line(line) {}

        token_type m_type;
        std::string m_lexeme;
        t_literal m_literal;
        u32 m_line;
    };
}