#include "compilation/lexer.h"
#include <iostream>

namespace dconstruct::compilation {

[[nodiscard]] const std::vector<token>& Lexer::scan_tokens() {
    while (!reached_eof()) {
        m_start = m_current;
        token t = scan_token();
        if (t.m_type != token_type::EMPTY) {
            m_tokens.push_back(std::move(t));
        }
    }

    m_tokens.emplace_back(token_type::_EOF, "", 0, m_line);

    return m_tokens;
}



[[nodiscard]] bool Lexer::reached_eof() const noexcept {
    return m_current >= m_source.size();
}

[[nodiscard]] token Lexer::make_current_token(const token_type type, const ast::primitive_value& literal) const {
    const std::string text = make_current_lexeme();
    return token(type, text, literal, m_line);
}

[[nodiscard]] std::string Lexer::make_current_lexeme() const {
    return m_source.substr(m_start, m_current - m_start);
}

char Lexer::advance() {
    return m_source.at(m_current++);
}

[[nodiscard]] bool Lexer::match(const char expected) {
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

[[nodiscard]] bool Lexer::is_valid_sid_char(const char c) const noexcept {
    // https://docs.racket-lang.org/guide/syntax-overview.html#%28part._.Identifiers%29

    return c < 127 && c > 32 && c != '#' && 
        c != '(' && c != ')' && c != '{' && c != '}' &&
        c != '[' && c != ']' && c != ',' && c != '"' &&
        c != '`' && c != '\n' && c != ';' && c != '|' &&
        c != '\\';  
}

[[nodiscard]] token Lexer::make_sid() {
    while (is_valid_sid_char(peek())) {
        advance();
    }
    std::string literal = m_source.substr(m_start + 1, m_current - m_start - 1);

    sid64 sid_hex_value = 0;
    sid64_literal result = {0, ""};

    auto [ptr, ec] = std::from_chars(literal.data(), literal.data() + literal.size(), sid_hex_value, 16);
    if (ec == std::errc{} && ptr == literal.data() + literal.size()) {
        result.first = sid_hex_value;
    } else {
        result.second = std::move(literal);
    }

    return make_current_token(token_type::SID, std::move(result));
}

[[nodiscard]] token Lexer::make_number() {
    bool is_double = false;
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
        return make_current_token(token_type::DOUBLE, std::stof(make_current_lexeme()));
    }
    const u64 number = std::stoull(make_current_lexeme());

    if (number <= std::numeric_limits<u16>::max()) {
        return make_current_token(token_type::INT, static_cast<u16>(number));
    } else if (number <= std::numeric_limits<u32>::max()) {
        return make_current_token(token_type::INT, static_cast<u32>(number));
    } else {
        return make_current_token(token_type::INT, static_cast<u64>(number));
    }
}

[[nodiscard]] bool Lexer::is_hex_char(const char c) const noexcept {
    return std::isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

[[nodiscard]] token Lexer::make_hex() {
    while (is_hex_char(peek())) {
        advance();
    }
    const i32 literal = std::stoi(make_current_lexeme(), 0, 16);
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
        case ':': return make_current_token(token_type::COLON);
        case '.': return make_current_token(token_type::DOT); 
        case ';': return make_current_token(token_type::SEMICOLON); 
        case '+': return make_current_token(match('=') ? token_type::PLUS_EQUAL : match('+') ? token_type::PLUS_PLUS : token_type::PLUS); 
        case '-': return make_current_token(match('=') ? token_type::MINUS_EQUAL : match('-') ? token_type::MINUS_MINUS : match('>') ? token_type::ARROW : token_type::MINUS); 
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
