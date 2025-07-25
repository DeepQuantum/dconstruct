#pragma once

#include "base.h"
#include "tokens.h"
#include <vector>
#include <ostream>
#include <iostream>
#include <unordered_map>
#include <sstream>

namespace dconstruct::compiler {

struct lexing_error {
    u32 m_line;
    std::string m_message;

    [[nodiscard]] b8 operator==(const lexing_error &rhs) const noexcept {
        return m_line == rhs.m_line && m_message == rhs.m_message;
    }
};


inline std::ostream& operator<<(std::ostream& os, const lexing_error &l) {
    return os << "line: " << l.m_line << " message: " << l.m_message;
}

inline std::ostream& operator<<(std::ostream& os, const token &t) {
    std::string literal_type, literal_value;
    std::visit([&](const auto& value) {
        std::ostringstream oss;
        literal_type = typeid(value).name(); 
        oss << value;
        literal_value = oss.str();
    }, t.m_literal);
    return os
        << "type: " << t.m_type
        << " lexeme: " << t.m_lexeme
        << " literal: " << literal_type << ' ' << literal_value
        << " line: " << t.m_line;
}


class Lexer {
public:
    Lexer(const std::string &source) : m_source(source) {};
    const std::vector<token>& scan_tokens() noexcept;
    const std::vector<lexing_error>& get_errors() const noexcept;

private:
    const std::unordered_map<std::string, token_type> m_keywords {
        {"else", token_type::ELSE},
        {"false", token_type::FALSE},
        {"if", token_type::IF},
        {"null", token_type::_NULL},
        {"return", token_type::RETURN},
        {"struct", token_type::STRUCT},
        {"true", token_type::TRUE},
        {"while", token_type::WHILE},
    };
    std::vector<token> m_tokens;
    std::vector<lexing_error> m_errors;
    std::string m_source;

    u32 m_start = 0;
    u32 m_current = 0;
    u32 m_line = 1;

    b8 m_hadError = false;

    [[nodiscard]] token scan_token() noexcept;
    [[nodiscard]] b8 reached_eof() const noexcept;
    char advance() noexcept;
    [[nodiscard]] std::string make_current_lexeme() const noexcept;
    [[nodiscard]] token make_current_token(const token_type, const token::t_literal& = 0ULL) const noexcept;
    [[nodiscard]] token make_string() noexcept;
    [[nodiscard]] token make_number() noexcept;
    [[nodiscard]] token make_hex() noexcept;
    [[nodiscard]] token make_identifier() noexcept;
    [[nodiscard]] token make_sid() noexcept;
    [[nodiscard]] b8 is_sid_char(const char) const noexcept;
    [[nodiscard]] b8 is_hex_char(const char) const noexcept;
    [[nodiscard]] b8 match(const char) noexcept;
    [[nodiscard]] char peek() const noexcept;
    [[nodiscard]] char peek_next() const noexcept;
};

}