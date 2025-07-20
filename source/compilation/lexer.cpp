#include "lexer.h"
#include <iostream>

namespace dconstruct::compiler {

[[nodiscard]] const std::vector<token>& Lexer::scan_tokens() noexcept {
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

[[nodiscard]] const std::vector<lexing_error>& Lexer::get_errors() const noexcept {
    return m_errors;
}

[[nodiscard]] b8 Lexer::reached_eof() const noexcept {
    return m_current >= m_source.size();
}

[[nodiscard]] token Lexer::make_current_token(const token_type type, const token::t_literal& literal) const noexcept {
    const std::string text = make_current_lexeme();
    return token(type, text, literal, m_line);
}

[[nodiscard]] std::string Lexer::make_current_lexeme() const noexcept {
    return m_source.substr(m_start, m_current - m_start);
}

char Lexer::advance() noexcept {
    return m_source.at(m_current++);
}

[[nodiscard]] b8 Lexer::match(const char expected) noexcept {
    if (reached_eof()) {
        return false;
    }
    if (m_source.at(m_current) != expected) {
        return false;
    }
    m_current++;
    return true;
}

[[nodiscard]] char Lexer::peek() const noexcept {
    if (reached_eof()) {
        return '\0';
    }
    return m_source.at(m_current);
}

[[nodiscard]] char Lexer::peek_next() const noexcept {
    if (m_current + 1 >= m_source.size()) {
        return '\0';
    }
    return m_source.at(m_current + 1);
}

[[nodiscard]] token Lexer::make_string() noexcept {
    while (peek() != '"' && !reached_eof()) {
        if (peek() == '\n') {
            m_line++;
        }
        advance();
    }
    if (reached_eof()) {
        m_errors.emplace_back(m_line, "unterminated string literal");
        return token(EMPTY, "");
    }
    advance();
    const std::string lexeme = make_current_lexeme();
    const std::string literal = m_source.substr(m_start + 1, m_current - m_start - 2);
    return token(STRING, lexeme, literal, m_line);
}

[[nodiscard]] b8 Lexer::is_sid_char(const char c) const noexcept {
    return std::isdigit(c) || std::isalpha(c) || c == '-';
}



[[nodiscard]] token Lexer::make_sid() noexcept {
    char current = peek();
    while (is_sid_char(current)) {
        advance();
        current = peek();
    }
    const std::string literal = m_source.substr(m_start + 1, m_current - m_start);
    return make_current_token(SID, literal);
}

[[nodiscard]] token Lexer::make_number() noexcept {
    b8 is_double = false;
    b8 is_hex = false;
    if (peek() == 'x' || peek() == 'X') {
        is_hex = true;
        advance();
    }
    while (std::isdigit(peek())) {
        advance();
    }
    if (peek() == '.' && std::isdigit(peek_next()) && !is_hex) {
        is_double = true;
        advance();
        while (std::isdigit(peek())) {
            advance();
        }
    }
    if (is_hex) {
        const u64 literal = std::stoull(make_current_lexeme(), 0, 16);
        return make_current_token(HEX, literal);
    }
    if (is_double) {
        return make_current_token(DOUBLE, std::stod(make_current_lexeme()));
    }
    return make_current_token(INT, std::stoull(make_current_lexeme()));
}

[[nodiscard]] token Lexer::make_identifier() noexcept {
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
    return make_current_token(IDENTIFIER);
}

[[nodiscard]] token Lexer::scan_token() noexcept {
    const char c = advance();
    switch(c) {
        case '(': return make_current_token(LEFT_PAREN); 
        case ')': return make_current_token(RIGHT_PAREN); 
        case '{': return make_current_token(LEFT_BRACE); 
        case '}': return make_current_token(RIGHT_BRACE); 
        case '[': return make_current_token(LEFT_SQUARE); 
        case ']': return make_current_token(RIGHT_SQUARE); 
        case ',': return make_current_token(COMMA); 
        case '.': return make_current_token(DOT); 
        case ';': return make_current_token(SEMICOLON); 
        case '+': return make_current_token(match('=') ? PLUS_EQUAL : match('+') ? PLUS_PLUS : PLUS); 
        case '-': return make_current_token(match('=') ? MINUS_EQUAL :  match('-') ? MINUS_MINUS : MINUS); 
        case '*': return make_current_token(match('=') ? STAR_EQUAL : STAR);
        case '!': return make_current_token(match('=') ? BANG_EQUAL : BANG);
        case '=': return make_current_token(match('=') ? EQUAL_EQUAL : EQUAL);
        case '<': return make_current_token(match('=') ? LESS_EQUAL : LESS);
        case '>': return make_current_token(match('=') ? GREATER_EQUAL : GREATER);
        case '|': return make_current_token(match('|') ? PIPE_PIPE : PIPE);
        case '^': return make_current_token(match('^') ? CARET_CARET : CARET);
        case '&': return make_current_token(match('&') ? AMPERSAND_AMPERSAND : AMPERSAND);
        case '/': {
            if (match('/')) {
                while (peek() != '\n' && !reached_eof()) {
                    advance();
                }
                break;
            } else if (match('=')) {
                return make_current_token(SLASH_EQUAL);
            } else {
                return make_current_token(SLASH);
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
                return make_number();
            } else if (std::isalpha(c) || c == '_') {
                return make_identifier();
            }
            m_errors.emplace_back(m_line, std::string("invalid token '") + c + '\'');
            break;
        }
    }
    return token(EMPTY, "");
}

}
