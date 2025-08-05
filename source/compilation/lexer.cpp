#include "compilation/lexer.h"
#include <iostream>

namespace dconstruct::compiler {

[[nodiscard]] const std::vector<token>& Lexer::scan_tokens() {
    while (!reached_eof()) {
        m_start = m_current;
        token t = scan_token();
        if (t.m_type != token_type::EMPTY) {
            m_tokens.push_back(std::move(t));
        }
    }

    m_tokens.emplace_back(token_type::_EOF, "", 0ULL, m_line);

    return m_tokens;
}



[[nodiscard]] b8 Lexer::reached_eof() const noexcept {
    return m_current >= m_source.size();
}

[[nodiscard]] token Lexer::make_current_token(const token_type type, const token::t_literal& literal) const {
    const std::string text = make_current_lexeme();
    return token(type, text, literal, m_line);
}

[[nodiscard]] std::string Lexer::make_current_lexeme() const {
    return m_source.substr(m_start, m_current - m_start);
}

char Lexer::advance() {
    return m_source.at(m_current++);
}

[[nodiscard]] b8 Lexer::match(const char expected) {
    if (reached_eof()) {
        return false;
    }
    if (m_source.at(m_current) != expected) {
        return false;
    }
    m_current++;
    return true;
}

[[nodiscard]] char Lexer::peek() const {
    if (reached_eof()) {
        return '\0';
    }
    return m_source.at(m_current);
}

[[nodiscard]] char Lexer::peek_next() const {
    if (m_current + 1 >= m_source.size()) {
        return '\0';
    }
    return m_source.at(m_current + 1);
}

[[nodiscard]] token Lexer::make_string() {
    while (peek() != '"' && !reached_eof()) {
        if (peek() == '\n') {
            m_line++;
        }
        advance();
    }
    if (reached_eof()) {
        m_errors.emplace_back(m_line, "unterminated string literal");
        return token(token_type::EMPTY, "");
    }
    advance();
    const std::string lexeme = make_current_lexeme();
    const std::string literal = m_source.substr(m_start + 1, m_current - m_start - 2);
    return token(token_type::STRING, lexeme, literal, m_line);
}

[[nodiscard]] b8 Lexer::is_sid_char(const char c) const noexcept {
    return std::isdigit(c) || std::isalpha(c) || c == '-' || c == '_';
}



[[nodiscard]] token Lexer::make_sid() {
    while (is_sid_char(peek())) {
        advance();
    }
    const std::string literal = m_source.substr(m_start + 1, m_current - m_start - 1);
    return make_current_token(token_type::SID, literal);
}

[[nodiscard]] token Lexer::make_number() {
    b8 is_double = false;
    while (std::isdigit(peek())) {
        advance();
    }
    if (peek() == '.' && std::isdigit(peek_next())) {
        is_double = true;
        advance();
        while (std::isdigit(peek())) {
            advance();
        }
    }
    if (is_double) {
        return make_current_token(token_type::DOUBLE, std::stod(make_current_lexeme()));
    }
    return make_current_token(token_type::INT, std::stoull(make_current_lexeme()));
}

[[nodiscard]] b8 Lexer::is_hex_char(const char c) const noexcept {
    return std::isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

[[nodiscard]] token Lexer::make_hex() {
    while (is_hex_char(peek())) {
        advance();
    }
    const u64 literal = std::stoull(make_current_lexeme(), 0, 16);
    return make_current_token(token_type::HEX, literal);
}

[[nodiscard]] token Lexer::make_identifier() {
    char current = peek();
    while (std::isalpha(current) || std::isdigit(current) || current == '_') {
        advance();
        current = peek();
    }
    const std::string identifier = make_current_lexeme();
    if (m_keywords.contains(identifier.c_str())) {
        const token_type keyword_token_type = m_keywords.at(identifier.c_str());
        return make_current_token(keyword_token_type);
    }
    return make_current_token(token_type::IDENTIFIER);
}

[[nodiscard]] token Lexer::scan_token() {
    const char c = advance();
    switch(c) {
        case '(': return make_current_token(token_type::LEFT_PAREN); 
        case ')': return make_current_token(token_type::RIGHT_PAREN); 
        case '{': return make_current_token(token_type::LEFT_BRACE); 
        case '}': return make_current_token(token_type::RIGHT_BRACE); 
        case '[': return make_current_token(token_type::LEFT_SQUARE); 
        case ']': return make_current_token(token_type::RIGHT_SQUARE); 
        case ',': return make_current_token(token_type::COMMA); 
        case '.': return make_current_token(token_type::DOT); 
        case ';': return make_current_token(token_type::SEMICOLON); 
        case '+': return make_current_token(match('=') ? token_type::PLUS_EQUAL : match('+') ? token_type::PLUS_PLUS : token_type::PLUS); 
        case '-': return make_current_token(match('=') ? token_type::MINUS_EQUAL :  match('-') ? token_type::MINUS_MINUS : token_type::MINUS); 
        case '*': return make_current_token(match('=') ? token_type::STAR_EQUAL : token_type::STAR);
        case '!': return make_current_token(match('=') ? token_type::BANG_EQUAL : token_type::BANG);
        case '=': return make_current_token(match('=') ? token_type::EQUAL_EQUAL : token_type::EQUAL);
        case '<': return make_current_token(match('=') ? token_type::LESS_EQUAL : token_type::LESS);
        case '>': return make_current_token(match('=') ? token_type::GREATER_EQUAL : token_type::GREATER);
        case '|': return make_current_token(match('|') ? token_type::PIPE_PIPE : token_type::PIPE);
        case '^': return make_current_token(match('^') ? token_type::CARET_CARET : token_type::CARET);
        case '&': return make_current_token(match('&') ? token_type::AMPERSAND_AMPERSAND : token_type::AMPERSAND);
        case '/': {
            if (match('/')) {
                while (peek() != '\n' && !reached_eof()) {
                    advance();
                }
                break;
            } else if (match('=')) {
                return make_current_token(token_type::SLASH_EQUAL);
            } else {
                return make_current_token(token_type::SLASH);
            }
        }
        case ' ':
        case '\r':
        case '\t': {
            break;
        }
        case '\n': {
            m_line++;
            break;
        }
        case '#': return make_sid();
        case '"': return make_string();
        default: {
            if (std::isdigit(c)) {
                if (match('X') || match('x')) {
                    return make_hex();
                }
                return make_number();
            } else if (std::isalpha(c) || c == '_') {
                return make_identifier();
            }
            m_errors.emplace_back(m_line, std::string("invalid token '") + c + '\'');
            break;
        }
    }
    return token(token_type::EMPTY, "");
}

}
