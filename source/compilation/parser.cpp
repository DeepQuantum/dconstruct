#include "compilation/dc_parser.h"
#include <vector>

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

[[nodiscard]] std::vector<stmnt_uptr> Parser::parse() {
    std::vector<stmnt_uptr> statements;
    while (!is_at_end()) {
        stmnt_uptr declaration = make_declaration();
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
        m_errors.emplace_back(peek(), "unknown type '" + peek().m_lexeme + "'");
        return nullptr;
    }
    const std::string type_name = advance().m_lexeme;
    const token* name = consume(token_type::IDENTIFIER, "expected variable name.");
    if (name == nullptr) {
        return nullptr;
    }

    expr_uptr init = nullptr;
    if (match({token_type::EQUAL})) {
        init = make_expression();
    }
    if (consume(token_type::SEMICOLON, "expected ';' after variable declaration.") == nullptr) {
        return nullptr;
    }
    if (init != nullptr) {
        return std::make_unique<ast::variable_declaration>(type_name, name->m_lexeme, std::move(init));
    } else {
        return std::make_unique<ast::variable_declaration>(type_name, name->m_lexeme); 
    }
}


[[nodiscard]] stmnt_uptr Parser::make_declaration() {
    stmnt_uptr res;
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

[[nodiscard]] std::unique_ptr<ast::while_stmt> Parser::make_while() {
    if (consume(token_type::LEFT_PAREN, "expected '(' after 'while'.") == nullptr) {
        return nullptr;
    }
    expr_uptr condition = make_expression();
    if (condition == nullptr) {
        return nullptr;
    }
    if (consume(token_type::RIGHT_PAREN, "expected ')' after while-condition") == nullptr) {
        return nullptr;
    }
    stmnt_uptr body = make_statement();
    if (body == nullptr) {
        return nullptr;
    }
    return std::make_unique<ast::while_stmt>(std::move(condition), std::move(body));
}

[[nodiscard]] stmnt_uptr Parser::make_for() {
    if (consume(token_type::LEFT_PAREN, "expected '(' after 'for'.") == nullptr) {
        return nullptr;
    }
    stmnt_uptr initializer = nullptr;
    if (match({token_type::SEMICOLON})) {
        initializer = nullptr;
    } else if (match_type()) {
        initializer = make_var_declaration();
    } else {
        initializer = make_expression_statement();
    }
    if (initializer == nullptr) {
        return nullptr;
    }
    expr_uptr condition = nullptr;
    if (!check(token_type::SEMICOLON)) {
        condition = make_expression();
    }
    if (condition == nullptr) {
        return nullptr;
    }
    if (consume(token_type::SEMICOLON, "expected ';' after loop condition.") == nullptr) {
        return nullptr;
    }
    expr_uptr increment = nullptr;
    if (!check(token_type::RIGHT_PAREN)) {
        increment = make_expression();
    }
    if (increment == nullptr) {
        return nullptr;
    }
    if (consume(token_type::RIGHT_PAREN, "expected ')' at end of for-loop header") == nullptr) {
        return nullptr;
    }
    stmnt_uptr body = make_statement();
    if (body == nullptr) {
        return nullptr;
    }
    if (increment != nullptr) {
        std::vector<stmnt_uptr> statements{};  
        statements.push_back(std::move(body));
        statements.push_back(std::make_unique<ast::expression_stmt>(std::move(increment)));
        body = std::make_unique<ast::block>(std::move(statements));
    }

    if (condition == nullptr) {
        condition = std::make_unique<ast::literal>(true);
    }
    body = std::make_unique<ast::while_stmt>(std::move(condition), std::move(body));

    if (initializer != nullptr) {
        std::vector<stmnt_uptr> statements{};
        statements.push_back(std::move(initializer));
        statements.push_back(std::move(body));
        body = std::make_unique<ast::block>(std::move(statements));
    }

    return body;
}

[[nodiscard]] stmnt_uptr Parser::make_statement() {
    if (match({token_type::IF})) {
        return make_if();
    } else if (match({token_type::WHILE})) {
        return make_while();
    } else if (match({token_type::FOR})) {
        return make_for();
    } else if (match({token_type::LEFT_BRACE})) {
        return make_block();
    } else {
        return make_expression_statement();
    }
}

[[nodiscard]] std::unique_ptr<ast::if_stmt> Parser::make_if() {
    if (consume(token_type::LEFT_PAREN, "expected '(' after 'if'.") == nullptr) {
        return nullptr;
    }
    expr_uptr condition = make_expression();
    if (condition == nullptr) {
        return nullptr;
    }
    if (consume(token_type::RIGHT_PAREN, "expect ')' after if-condition.") == nullptr) {
        return nullptr;
    }
    stmnt_uptr then_branch = make_statement();
    if (then_branch == nullptr) {
        return nullptr;
    }
    stmnt_uptr else_branch = nullptr;
    if (match({token_type::ELSE})) {
        else_branch = make_statement();
        if (else_branch == nullptr) {
            return nullptr;
        }
    }

    return std::make_unique<ast::if_stmt>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

[[nodiscard]] std::unique_ptr<ast::block> Parser::make_block() {
    std::vector<stmnt_uptr> statements{};
    while (!check(token_type::RIGHT_BRACE) && !is_at_end()) {
        statements.push_back(make_declaration());
    }
    if (consume(token_type::RIGHT_BRACE, "expected '}' after block.") == nullptr) {
        return nullptr;
    }
    return std::make_unique<ast::block>(std::move(statements));
}

[[nodiscard]] std::unique_ptr<ast::expression_stmt> Parser::make_expression_statement() {
    expr_uptr expr = make_expression();
    if (expr == nullptr) {
        return nullptr;
    }
    const token *matched = consume(token_type::SEMICOLON, "expected ';' after expression.");
    if (matched == nullptr) {
        return nullptr;
    }
    return std::make_unique<ast::expression_stmt>(std::move(expr));
}

[[nodiscard]] expr_uptr Parser::make_or() {
    expr_uptr expr = make_and();
    if (expr == nullptr) {
        return nullptr;
    }
    while (match({token_type::OR, token_type::PIPE_PIPE})) {
        const token op = previous();
        expr_uptr right = make_and();
        if (right == nullptr) {
            return nullptr;
        }
        expr = std::make_unique<ast::logical_expr>(op, std::move(expr), std::move(right));
    }
    return expr;
}

[[nodiscard]] expr_uptr Parser::make_and() {
    expr_uptr expr = make_equality();

    while (match({token_type::AND, token_type::AMPERSAND_AMPERSAND})) {
        const token op = previous();
        expr_uptr right = make_equality();
        expr = std::make_unique<ast::logical_expr>(op, std::move(expr), std::move(right));
    }
    return expr;
}

[[nodiscard]] expr_uptr Parser::make_assignment() {
    expr_uptr expr = make_or();
    if (expr == nullptr) {
        return nullptr;
    }
    if (match({token_type::EQUAL})) {
        const token equals = previous();
        expr_uptr value = make_assignment();
        const ast::identifier* expr_ptr = dynamic_cast<const ast::identifier*>(expr.get());
        if (expr_ptr != nullptr) {
            const token name = expr_ptr->m_name;
            return std::make_unique<ast::assign_expr>(name, std::move(value));
        }

        m_errors.emplace_back(equals, "invalid assignment target.");
        return nullptr;
    }
    return expr;
}

[[nodiscard]] expr_uptr Parser::make_expression() {
    return make_assignment();
}

[[nodiscard]] expr_uptr Parser::make_equality() {
    expr_uptr expr = make_comparison();
    if (expr == nullptr) {
        return nullptr;
    }
    while (match({token_type::BANG_EQUAL, token_type::EQUAL_EQUAL})) {
        const token& op = previous();
        expr_uptr right = make_comparison();
        if (right == nullptr) {
            return nullptr;
        }
        switch (op.m_type) {
            case token_type::BANG_EQUAL: {
                expr = std::make_unique<ast::compare_expr>(op, std::move(expr), std::move(right));
                break;
            }
            case token_type::EQUAL_EQUAL: {
                expr = std::make_unique<ast::compare_expr>(op, std::move(expr), std::move(right));
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

[[nodiscard]] expr_uptr Parser::make_comparison() {
    expr_uptr expr = make_term();
    if (expr == nullptr) {
        return nullptr;
    }
    while (match({token_type::GREATER, token_type::GREATER_EQUAL, token_type::LESS, token_type::LESS_EQUAL})) {
        const token& op = previous();
        expr_uptr right = make_term();
        if (right == nullptr) {
            return nullptr;
        }
        switch (op.m_type) {
            case token_type::GREATER: {
                expr = std::make_unique<ast::compare_expr>(op, std::move(expr), std::move(right));
                break;
            }
            case token_type::GREATER_EQUAL: {
                expr = std::make_unique<ast::compare_expr>(op, std::move(expr), std::move(right));
                break;
            }
            case token_type::LESS: {
                expr = std::make_unique<ast::compare_expr>(op, std::move(expr), std::move(right));
                break;
            }
            case token_type::LESS_EQUAL: {
                expr = std::make_unique<ast::compare_expr>(op, std::move(expr), std::move(right));
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

[[nodiscard]] expr_uptr Parser::make_term() {
    expr_uptr expr = make_factor();
    if (expr == nullptr) {
        return nullptr;
    }
    while (match({token_type::MINUS, token_type::PLUS})) {
        const token& op = previous();
        expr_uptr right = make_factor();
        if (right == nullptr) {
            return nullptr;
        }
        switch (op.m_type) {
            case token_type::PLUS: {
                expr = std::make_unique<ast::add_expr>(op, std::move(expr), std::move(right));
                break;
            }
            case token_type::MINUS: {
                expr = std::make_unique<ast::sub_expr>(op, std::move(expr), std::move(right));
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

[[nodiscard]] expr_uptr Parser::make_factor() {
    expr_uptr expr = make_unary();
    while (match({token_type::SLASH, token_type::STAR})) {
        const token& op = previous();
        expr_uptr right = make_unary();
        if (right == nullptr) {
            return nullptr;
        }
        switch (op.m_type) {
            case token_type::SLASH: {
                expr = std::make_unique<ast::div_expr>(op, std::move(expr), std::move(right));
                break;
            }
            case token_type::STAR: {
                expr = std::make_unique<ast::mul_expr>(op, std::move(expr), std::move(right));
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

[[nodiscard]] expr_uptr Parser::make_unary() {
    if (match({token_type::BANG, token_type::MINUS})) {
        const token& op = previous();
        expr_uptr right = make_unary();
        if (right == nullptr) {
            return nullptr;
        }
        switch (op.m_type) {
            case token_type::BANG: {
                return std::make_unique<ast::logical_not_expr>(op, std::move(right));
            }
            default: {
                m_errors.emplace_back(op, "unexpected token " + op.m_lexeme);
                return nullptr;
            }
        }
    }
    return make_primary();
}

[[nodiscard]] expr_uptr Parser::make_primary() {
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
        const sid_literal sid = { 0, std::get<std::string>(previous().m_literal) };
        return std::make_unique<ast::literal>(sid);
    } else if (match({token_type::IDENTIFIER})) {
        return std::make_unique<ast::identifier>(previous());
    }
    if (match({token_type::LEFT_PAREN})) {
        std::unique_ptr expr = make_expression();
        if(consume(token_type::RIGHT_PAREN, "expected ')' after expression") == nullptr) {
            return nullptr;
        }
        return std::make_unique<ast::grouping>(std::move(expr));
    }

    m_errors.emplace_back(previous(), "expected expression after '" + previous().m_lexeme + "'");
    return nullptr;
}


}