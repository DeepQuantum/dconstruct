#include "lexer.h"
#include <iostream>

namespace dconstruct::compiler {

std::vector<token>& Lexer::scan_tokens() noexcept {
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

const std::vector<lexing_error>& Lexer::get_errors() const noexcept {
    return m_errors;
}

b8 Lexer::reached_eof() const noexcept {
    return m_current >= m_source.size();
}

token Lexer::make_current_token(const token_type type, const token::t_literal& literal) const noexcept {
    const std::string text = m_source.substr(m_start, m_current - m_start);
    std::string message = "hello";
    return token(type, text, literal, m_line);
}

char Lexer::advance() noexcept {
    return m_source.at(m_current++);
}

b8 Lexer::match(const char expected) noexcept {
    if (reached_eof()) {
        return false;
    }
    if (m_source.at(m_current) != expected) {
        return false;
    }
    m_current++;
    return true;
}

char Lexer::peek() const noexcept {
    if (reached_eof()) {
        return '\0';
    }
    return m_source.at(m_current);
}

char Lexer::peek_next() const noexcept {
    if (m_current + 1 >= m_source.size()) {
        return '\0';
    }
    return m_source.at(m_current + 1);
}

token Lexer::make_string() noexcept {
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
    const std::string lexeme = m_source.substr(m_start, m_current - m_start);
    const std::string literal = m_source.substr(m_start + 1, m_current - m_start - 2);
    return token(STRING, lexeme, literal, m_line);
}

token Lexer::make_number() noexcept {
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
        return make_current_token(DOUBLE, std::stod(m_source.substr(m_start, m_current - m_start)));
    }
    return make_current_token(INT, std::stoull(m_source.substr(m_start, m_current - m_start)));
}

token Lexer::scan_token() noexcept {
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
        case '-': return make_current_token(MINUS); 
        case '+': return make_current_token(PLUS); 
        case ';': return make_current_token(SEMICOLON); 
        case '*': return make_current_token(STAR);
        case '!': return make_current_token(match('=') ? BANG_EQUAL : BANG);
        case '=': return make_current_token(match('=') ? EQUAL_EQUAL : EQUAL);
        case '<': return make_current_token(match('=') ? LESS_EQUAL : LESS);
        case '>': return make_current_token(match('=') ? GREATER_EQUAL : GREATER);
        case '/': {
            if (match('/')) {
                while (peek() != '\n' && !reached_eof()) {
                    advance();
                }
                break;
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
        case '"': return make_string();
        default: {
            if (std::isdigit(c)) {
                return make_number();
            }
            m_errors.emplace_back(m_line, "invalid token " + c);
            break;
        }
    }
    return token(EMPTY, "");
}

}