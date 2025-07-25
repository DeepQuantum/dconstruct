#include "dc_parser.h"
#include "literal.h"

namespace dconstruct::compiler {

[[nodiscard]] const token& Parser::peek() const noexcept {
    return m_tokens[m_current];
}

[[nodiscard]] const token& Parser::previous() const noexcept {
    return m_tokens[m_current - 1];
}

[[nodiscard]] b8 Parser::is_at_end() const noexcept {
    return peek().m_type == token_type::_EOF;
}

const token& Parser::advance() noexcept {
    if (!is_at_end()) {
        m_current++;
    }
    return previous();
}

const token* Parser::consume(const token_type type, const std::string& message) noexcept {
    if (check(type)) {
        return &advance();
    }
    m_errors.emplace_back(peek(), message);
    return nullptr;
}

[[nodiscard]] b8 Parser::check(const token_type type) const noexcept {
    if (is_at_end()) {
        return false;
    }
    return peek().m_type == type;
}

[[nodiscard]] b8 Parser::match(const std::initializer_list<token_type>& types) noexcept {
    for (const token_type tt : types) {
        if (check(tt)) {
            advance();
            return true;
        }
    }
    return false;
}

[[nodiscard]] std::unique_ptr<ast::expression> Parser::parse() noexcept {
    return make_expression();
}

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_expression() noexcept {
    return make_equality();
}

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_equality() noexcept {
    std::unique_ptr<ast::expression> expr = make_comparison();
    if (expr == nullptr) {
        return nullptr;
    }
    while (match({BANG_EQUAL, EQUAL_EQUAL})) {
        const token& op = previous();
        std::unique_ptr<ast::expression> right = make_comparison();
        if (right == nullptr) {
            return nullptr;
        }
        switch (op.m_type) {
            case token_type::BANG_EQUAL: {
                expr = std::make_unique<ast::compare_expr>(ast::compare_expr::comp_type::NEQ, std::move(expr), std::move(right));
                break;
            }
            case token_type::EQUAL_EQUAL: {
                expr = std::make_unique<ast::compare_expr>(ast::compare_expr::comp_type::EQ, std::move(expr), std::move(right));
                break;
            }
            default: {
                m_errors.emplace_back(op, "unexpected token " + op.m_lexeme);
                return nullptr;
            }
        }
    }
    return expr;
}

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_comparison() noexcept {
    std::unique_ptr<ast::expression> expr = make_term();
    if (expr == nullptr) {
        return nullptr;
    }
    while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
        const token& op = previous();
        std::unique_ptr<ast::expression> right = make_term();
        if (right == nullptr) {
            return nullptr;
        }
        switch (op.m_type) {
            case GREATER: {
                expr = std::make_unique<ast::compare_expr>(ast::compare_expr::comp_type::GT, std::move(expr), std::move(right));
                break;
            }
            case GREATER_EQUAL: {
                expr = std::make_unique<ast::compare_expr>(ast::compare_expr::comp_type::GET, std::move(expr), std::move(right));
                break;
            }
            case LESS: {
                expr = std::make_unique<ast::compare_expr>(ast::compare_expr::comp_type::LT, std::move(expr), std::move(right));
                break;
            }
            case LESS_EQUAL: {
                expr = std::make_unique<ast::compare_expr>(ast::compare_expr::comp_type::LET, std::move(expr), std::move(right));
                break;
            }
            default: {
                m_errors.emplace_back(op, "unexpected token " + op.m_lexeme);
                return nullptr;
            }
        }
    }
    return expr;
}

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_term() noexcept {
    std::unique_ptr<ast::expression> expr = make_factor();
    if (expr == nullptr) {
        return nullptr;
    }
    while (match({MINUS, PLUS})) {
        const token& op = previous();
        std::unique_ptr<ast::expression> right = make_factor();
        if (right == nullptr) {
            return nullptr;
        }
        switch (op.m_type) {
            case PLUS: {
                expr = std::make_unique<ast::add_expr>(std::move(expr), std::move(right));
                break;
            }
            case MINUS: {
                expr = std::make_unique<ast::sub_expr>(std::move(expr), std::move(right));
                break;
            }
            default: {
                m_errors.emplace_back(op, "unexpected token " + op.m_lexeme);
                return nullptr;
            }
        }
    }
    return expr;
}

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_factor() noexcept {
    std::unique_ptr<ast::expression> expr = make_unary();
    while (match({SLASH, STAR})) {
        const token& op = previous();
        std::unique_ptr<ast::expression> right = make_unary();
        if (right == nullptr) {
            return nullptr;
        }
        switch (op.m_type) {
            case SLASH: {
                expr = std::make_unique<ast::div_expr>(std::move(expr), std::move(right));
                break;
            }
            case STAR: {
                expr = std::make_unique<ast::mul_expr>(std::move(expr), std::move(right));
                break;
            }
            default: {
                m_errors.emplace_back(op, "unexpected token " + op.m_lexeme);
                return nullptr;
            }
        }
    }
    return expr;
}

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_unary() noexcept {
    if (match({BANG, MINUS})) {
        const token& op = previous();
        std::unique_ptr<ast::expression> right = make_unary();
        if (right == nullptr) {
            return nullptr;
        }
        switch (op.m_type) {
            case BANG: {
                return std::make_unique<ast::logical_not_expr>(std::move(right));
            }
            default: {
                m_errors.emplace_back(op, "unexpected token " + op.m_lexeme);
                return nullptr;
            }
        }
    }
    return make_primary();
}

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_primary() noexcept {
    if (match({TRUE})) {
        return std::make_unique<ast::literal<b8>>(true);
    } else if (match({FALSE})) {
        return std::make_unique<ast::literal<b8>>(false);
    } else if (match({_NULL})) {
        return std::make_unique<ast::literal<std::nullptr_t>>(nullptr);
    } else if (match({INT})) {
        const u64 num = std::get<u64>(previous().m_literal);
        return std::unique_ptr<ast::literal<u64>>(new ast::literal<u64>(num));
    } else if (match({DOUBLE})) {
        const f64 num = std::get<f64>(previous().m_literal);
        return std::make_unique<ast::literal<f64>>(num);
    } else if (match({STRING})) {
        const std::string str = std::get<std::string>(previous().m_literal);
        return std::make_unique<ast::literal<std::string>>(str);
    }

    if (match({LEFT_PAREN})) {
        std::unique_ptr expr = make_expression();
        if(consume(RIGHT_PAREN, "error: expected ')' after expression") == nullptr) {
            return nullptr;
        }
        return std::make_unique<ast::grouping>(std::move(expr));
    }

    m_errors.emplace_back(peek(), "error: expected expression.");
    return nullptr;
}


}