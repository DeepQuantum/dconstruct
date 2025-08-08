#include "compilation/dc_parser.h"
#include "ast/primary_expressions/literal.h"
#include "ast/statements/expression_statement.h"

namespace dconstruct::compiler {

[[nodiscard]] const token& Parser::peek() const {
    return m_tokens[m_current];
}

[[nodiscard]] const token& Parser::previous() const {
    return m_tokens[m_current - 1];
}

[[nodiscard]] b8 Parser::is_at_end() const {
    return peek().m_type == token_type::_EOF;
}

const token& Parser::advance() {
    if (!is_at_end()) {
        m_current++;
    }
    return previous();
}

const token* Parser::consume(const token_type type, const std::string& message) {
    if (check(type)) {
        return &advance();
    }
    m_errors.emplace_back(peek(), message);
    return nullptr;
}

[[nodiscard]] b8 Parser::check(const token_type type) const {
    if (is_at_end()) {
        return false;
    }
    return peek().m_type == type;
}

[[nodiscard]] b8 Parser::match(const std::initializer_list<token_type>& types) {
    for (const token_type tt : types) {
        if (check(tt)) {
            advance();
            return true;
        }
    }
    return false;
}

[[nodiscard]] std::vector<std::unique_ptr<ast::statement>> Parser::parse() {
    std::vector<std::unique_ptr<ast::statement>> statements;
    while (!is_at_end()) {
        statements.push_back(make_statement());
    }
    return statements;
}

[[nodiscard]] std::unique_ptr<ast::statement> Parser::make_statement() {
    return make_expression_statement();
}

[[nodiscard]] std::unique_ptr<ast::statement> Parser::make_expression_statement() {
    std::unique_ptr<ast::expression> expr = make_expression();
    consume(token_type::SEMICOLON, "error: expected ';' after expression.");
    return std::make_unique<ast::expression_stmt>(std::move(expr));
}

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_expression() {
    return make_equality();
}

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_equality() {
    std::unique_ptr<ast::expression> expr = make_comparison();
    if (expr == nullptr) {
        return nullptr;
    }
    while (match({token_type::BANG_EQUAL, token_type::EQUAL_EQUAL})) {
        const token& op = previous();
        std::unique_ptr<ast::expression> right = make_comparison();
        if (right == nullptr) {
            return nullptr;
        }
        switch (op.m_type) {
            case token_type::BANG_EQUAL: {
                expr = std::make_unique<ast::compare_expr>(std::move(expr), std::move(right), ast::compare_expr::comp_type::NEQ);
                break;
            }
            case token_type::EQUAL_EQUAL: {
                expr = std::make_unique<ast::compare_expr>(std::move(expr), std::move(right), ast::compare_expr::comp_type::EQ);
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

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_comparison() {
    std::unique_ptr<ast::expression> expr = make_term();
    if (expr == nullptr) {
        return nullptr;
    }
    while (match({token_type::GREATER, token_type::GREATER_EQUAL, token_type::LESS, token_type::LESS_EQUAL})) {
        const token& op = previous();
        std::unique_ptr<ast::expression> right = make_term();
        if (right == nullptr) {
            return nullptr;
        }
        switch (op.m_type) {
            case token_type::GREATER: {
                expr = std::make_unique<ast::compare_expr>(std::move(expr), std::move(right), ast::compare_expr::comp_type::GT);
                break;
            }
            case token_type::GREATER_EQUAL: {
                expr = std::make_unique<ast::compare_expr>(std::move(expr), std::move(right), ast::compare_expr::comp_type::GET);
                break;
            }
            case token_type::LESS: {
                expr = std::make_unique<ast::compare_expr>(std::move(expr), std::move(right), ast::compare_expr::comp_type::LT);
                break;
            }
            case token_type::LESS_EQUAL: {
                expr = std::make_unique<ast::compare_expr>(std::move(expr), std::move(right), ast::compare_expr::comp_type::LET);
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

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_term() {
    std::unique_ptr<ast::expression> expr = make_factor();
    if (expr == nullptr) {
        return nullptr;
    }
    while (match({token_type::MINUS, token_type::PLUS})) {
        const token& op = previous();
        std::unique_ptr<ast::expression> right = make_factor();
        if (right == nullptr) {
            return nullptr;
        }
        switch (op.m_type) {
            case token_type::PLUS: {
                expr = std::make_unique<ast::add_expr>(std::move(expr), std::move(right));
                break;
            }
            case token_type::MINUS: {
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

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_factor() {
    std::unique_ptr<ast::expression> expr = make_unary();
    while (match({token_type::SLASH, token_type::STAR})) {
        const token& op = previous();
        std::unique_ptr<ast::expression> right = make_unary();
        if (right == nullptr) {
            return nullptr;
        }
        switch (op.m_type) {
            case token_type::SLASH: {
                expr = std::make_unique<ast::div_expr>(std::move(expr), std::move(right));
                break;
            }
            case token_type::STAR: {
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

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_unary() {
    if (match({token_type::BANG, token_type::MINUS})) {
        const token& op = previous();
        std::unique_ptr<ast::expression> right = make_unary();
        if (right == nullptr) {
            return nullptr;
        }
        switch (op.m_type) {
            case token_type::BANG: {
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

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_primary() {
    if (match({token_type::TRUE})) {
        return std::make_unique<ast::literal>(true);
    } else if (match({token_type::FALSE})) {
        return std::make_unique<ast::literal>(false);
    } else if (match({token_type::_NULL})) {
        return std::make_unique<ast::literal>(nullptr);
    } else if (match({token_type::INT})) {
        const u64 num = std::get<u64>(previous().m_literal);
        ast::literal* temp = new ast::literal(num);
        return std::unique_ptr<ast::literal>(new ast::literal(num));
    } else if (match({token_type::DOUBLE})) {
        const f64 num = std::get<f64>(previous().m_literal);
        return std::make_unique<ast::literal>(num);
    } else if (match({token_type::STRING})) {
        const std::string str = std::get<std::string>(previous().m_literal);
        return std::make_unique<ast::literal>(str);
    } else if (match({token_type::SID})) {
        
    }

    if (match({token_type::LEFT_PAREN})) {
        std::unique_ptr expr = make_expression();
        if(consume(token_type::RIGHT_PAREN, "error: expected ')' after expression") == nullptr) {
            return nullptr;
        }
        return std::make_unique<ast::grouping>(std::move(expr));
    }

    m_errors.emplace_back(previous(), "error: expected expression after '" + previous().m_lexeme + "'");
    return nullptr;
}


}