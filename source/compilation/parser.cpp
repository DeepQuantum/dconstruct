#include "compilation/dc_parser.h"

namespace dconstruct::compiler {

void Parser::synchronize() {
    advance();
    while (!is_at_end()) {
        if (previous().m_type == token_type::SEMICOLON) {
            return;
        }
        switch (peek().m_type) {
            case token_type::STRUCT:
            case token_type::WHILE:
            case token_type::IF:
            case token_type::RETURN: return;
            default: advance();
        }
    }
}

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
        std::unique_ptr<ast::statement> declaration = make_declaration();
        if (declaration != nullptr) {
            statements.push_back(std::move(declaration));
        }
    }
    return statements;
}


[[nodiscard]] b8 Parser::match_type() {
    return m_knownTypes.contains(peek().m_lexeme);
}

[[nodiscard]] std::unique_ptr<ast::variable_declaration> Parser::make_var_declaration() {
    if (!match_type()) {
        m_errors.emplace_back(peek(), "error: unknown type '" + peek().m_lexeme + "'");
        return nullptr;
    }
    const std::string type_name = advance().m_lexeme;
    const token* name = consume(token_type::IDENTIFIER, "error: expected variable name.");
    if (name == nullptr) {
        return nullptr;
    }

    std::unique_ptr<ast::expression> init = nullptr;
    if (match({token_type::EQUAL})) {
        init = make_expression();
    }
    if (consume(token_type::SEMICOLON, "error: expected ';' after variable declaration.") == nullptr) {
        return nullptr;
    }
    if (init != nullptr) {
        return std::make_unique<ast::variable_declaration>(type_name, name->m_lexeme, std::move(init));
    } else {
        return std::make_unique<ast::variable_declaration>(type_name, name->m_lexeme); 
    }
}


[[nodiscard]] std::unique_ptr<ast::statement> Parser::make_declaration() {
    std::unique_ptr<ast::statement> res;
    if (match_type()) {
        res = make_var_declaration();
    } else {
        res = make_statement();
    }
    if (res == nullptr) {
        synchronize();
        return nullptr;
    }
    return res;
}

[[nodiscard]] std::unique_ptr<ast::statement> Parser::make_statement() {
    if (match({token_type::LEFT_BRACE})) {
        return make_block();
    } else if (match({token_type::IF})) {
        return make_if();
    }
    return make_expression_statement();
}

[[nodiscard]] std::unique_ptr<ast::if_stmt> Parser::make_if() {
    if (consume(token_type::LEFT_PAREN, "error: expect '(' after 'if'.") == nullptr) {
        return nullptr;
    }
    std::unique_ptr<ast::expression> condition = make_expression();
    if (consume(token_type::RIGHT_PAREN, "error: expect ')' after if-condition.") == nullptr) {
        return nullptr;
    }
    std::unique_ptr<ast::statement> then_branch = make_statement();
    std::unique_ptr<ast::statement> else_branch = nullptr;
    if (match({token_type::ELSE})) {
        else_branch = make_statement();
    }

    return std::make_unique<ast::if_stmt>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

[[nodiscard]] std::unique_ptr<ast::block> Parser::make_block() {
    std::vector<std::unique_ptr<ast::statement>> statements{};
    while (!check(token_type::RIGHT_BRACE) && !is_at_end()) {
        statements.push_back(make_declaration());
    }
    consume(token_type::RIGHT_BRACE, "error: expected '}' after block.");
    return std::make_unique<ast::block>(std::move(statements));
}

[[nodiscard]] std::unique_ptr<ast::expression_stmt> Parser::make_expression_statement() {
    std::unique_ptr<ast::expression> expr = make_expression();
    const token *matched = consume(token_type::SEMICOLON, "error: expected ';' after expression.");
    if (matched == nullptr) {
        return nullptr;
    }
    return std::make_unique<ast::expression_stmt>(std::move(expr));
}

[[nodiscard]] std::unique_ptr<ast::expression> Parser::make_assignment() {
    std::unique_ptr<ast::expression> expr = make_equality();
    if (expr == nullptr) {
        return nullptr;
    }
    if (match({token_type::EQUAL})) {
        const token equals = previous();
        std::unique_ptr<ast::expression> value = make_assignment();
        const ast::identifier* value_ptr = dynamic_cast<const ast::identifier*>(value.get());
        if (value_ptr != nullptr) {
            const token name = value_ptr->m_name;
            return std::make_unique<ast::assign_expr>(name, std::move(value));
        }

        m_errors.emplace_back(equals, "error: invalid assignment target.");
    }
    return expr;
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
        const i32 num = std::get<i32>(previous().m_literal);
        return std::make_unique<ast::literal>(num);
    } else if (match({token_type::DOUBLE})) {
        const f64 num = std::get<f64>(previous().m_literal);
        return std::make_unique<ast::literal>(num);
    } else if (match({token_type::STRING})) {
        const std::string str = std::get<std::string>(previous().m_literal);
        return std::make_unique<ast::literal>(str);
    } else if (match({token_type::SID})) {
        const ast::sid_literal sid = { 0, std::get<std::string>(previous().m_literal) };
        return std::make_unique<ast::literal>(sid);
    } else if (match({token_type::IDENTIFIER})) {
        return std::make_unique<ast::identifier>(previous());
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