#include "parser.h"

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

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_expression() noexcept {
    return make_equality();
}

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_equality() noexcept {
    std::unique_ptr<ast::expression> expr = make_comparison();
    while (match({BANG_EQUAL, EQUAL_EQUAL})) {
        const token& op = previous();
        std::unique_ptr<ast::expression> right = make_comparison();
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
                
            }
        }
    }
    return expr;
}

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_comparison() noexcept {
    std::unique_ptr<ast::expression> expr = make_term();
    while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
        const token& op = previous();
        std::unique_ptr<ast::expression> right = make_term();
        switch (op.m_type) {
            case token_type::BANG_EQUAL: {
                expr = std::make_unique<ast::compare_expr>(ast::compare_expr::comp_type::NEQ, std::move(expr), std::move(right));
                break;
            }
            case token_type::EQUAL_EQUAL: {
                expr = std::make_unique<ast::compare_expr>(ast::compare_expr::comp_type::EQ, std::move(expr), std::move(right));
                break;
            }
        }
    }
    return expr;
}

}