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

[[nodiscard]] bool Parser::is_at_end() const {
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

[[nodiscard]] bool Parser::check(const token_type type) const {
    if (is_at_end()) {
        return false;
    }
    return peek().m_type == type;
}

[[nodiscard]] bool Parser::match(const std::initializer_list<token_type>& types) {
    for (const token_type tt : types) {
        if (check(tt)) {
            advance();
            return true;
        }
    }
    return false;
}

[[nodiscard]] std::optional<ast::full_type> Parser::peek_type() const {
    if (!check(token_type::IDENTIFIER)) {
        return std::nullopt;
    }

    const std::string type_name = peek().m_lexeme;
    if (m_knownTypes.contains(type_name)) {
        return std::nullopt;
    }

    ast::full_type res = m_knownTypes.at(type_name);

    u32 i = 0;
    while (m_tokens[m_current + i].m_type == token_type::STAR) {
        res = ast::ptr_type{std::make_unique<ast::full_type>(std::move(res))};
    }

    return res;
}

[[nodiscard]] std::optional<ast::full_type> Parser::make_type() {
    const token* type_token = consume(token_type::IDENTIFIER, "expected type name");
    if (!type_token) {
        return std::nullopt;
    }

    const std::string type_name = type_token->m_lexeme;
    if (m_knownTypes.contains(type_name)) {
        m_errors.emplace_back("unknown type " + type_name);
        return std::nullopt;
    }

    ast::full_type res = m_knownTypes.at(type_name);

    while (match({token_type::STAR}) && !is_at_end()) {
        res = ast::ptr_type{std::make_unique<ast::full_type>(std::move(res))};
    }

    return res;
}

[[nodiscard]] std::variant<ast::full_type, ast::function_definition> Parser::make_external_definition() {
    if (match({token_type::STRUCT})) {
        return *make_struct_type();
    } else if (match({token_type::ENUM})) {
        return *make_enum_type();
    }
}

[[nodiscard]] std::optional<ast::struct_type> Parser::make_struct_type() {
    const token* struct_name = consume(token_type::IDENTIFIER, "expected name after struct keyword");
    if (!struct_name) {
        return std::nullopt;
    }

    if (!consume(token_type::LEFT_BRACE, "expected '{'")) {
        return std::nullopt;
    }

    ast::struct_type struct_t;
    struct_t.m_name = struct_name->m_lexeme;

    while (!check(token_type::RIGHT_BRACE) && !is_at_end()) {
        std::unique_ptr<ast::variable_declaration> decl = make_var_declaration();
        if (!decl) {
            return std::nullopt;
        }
        struct_t.m_members[decl->m_identifier] = std::make_unique<ast::full_type>(std::move(decl->m_type));
    }

    if (!consume(token_type::RIGHT_BRACE, "expected '}' after struct definition")) {
        return std::nullopt;
    }

    return struct_t;
}

[[nodiscard]] std::optional<ast::enum_type> Parser::make_enum_type() {
    const token* enum_name = consume(token_type::IDENTIFIER, "expected name after enum keyword");
    if (!enum_name) {
        return std::nullopt;
    }

    if (!consume(token_type::LEFT_BRACE, "expected '{")) {
        return std::nullopt;
    }

    ast::enum_type enum_t;
    enum_t.m_name = enum_name->m_lexeme;

    while (!check(token_type::RIGHT_BRACE) && !is_at_end()) {
        const token* enum_value = consume(token_type::IDENTIFIER, "expected enumeration name");
        if (!enum_value) {
            return std::nullopt;
        }
        enum_t.m_enumerators.push_back(enum_value->m_lexeme);
    }

    if (!consume(token_type::RIGHT_BRACE, "expected '}' after struct definition")) {
        return std::nullopt;
    }

    return enum_t;
}

[[nodiscard]] std::vector<stmnt_uptr> Parser::parse() {
    std::vector<stmnt_uptr> statements;
    while (!is_at_end()) {
        stmnt_uptr declaration = make_declaration();
        if (declaration) {
            statements.push_back(std::move(declaration));
        }
    }
    return statements;
}


[[nodiscard]] const ast::full_type& Parser::make_type_from_string(const std::string& type_str) {
    if (!m_knownTypes.contains(type_str)) {
        return ast::full_type{std::monostate()};
    }
    return m_knownTypes[type_str];
}

[[nodiscard]] std::unique_ptr<ast::variable_declaration> Parser::make_var_declaration() {
    std::optional<ast::full_type> type = make_type();

    if (!type) {
        return nullptr;
    }
    
    const token* name = consume(token_type::IDENTIFIER, "expected variable name.");
    if (!name) {
        return nullptr;
    }

    expr_uptr init = nullptr;
    if (match({token_type::EQUAL})) {
        init = make_expression();
    }
    if (!consume(token_type::SEMICOLON, "expected ';' after variable declaration.")) {
        return nullptr;
    }

    if (init) {
        return std::make_unique<ast::variable_declaration>(std::move(*type), name->m_lexeme, std::move(init));
    } else {
        return std::make_unique<ast::variable_declaration>(std::move(*type), name->m_lexeme); 
    }
}

[[nodiscard]] std::unique_ptr<ast::variable_declaration> Parser::make_var_declaration(ast::full_type type) {
    const token* name = consume(token_type::IDENTIFIER, "expected variable name.");
    if (!name) {
        return nullptr;
    }

    expr_uptr init = nullptr;
    if (match({token_type::EQUAL})) {
        init = make_expression();
    }
    if (!consume(token_type::SEMICOLON, "expected ';' after variable declaration.")) {
        return nullptr;
    }

    if (init) {
        return std::make_unique<ast::variable_declaration>(std::move(type), name->m_lexeme, std::move(init));
    } else {
        return std::make_unique<ast::variable_declaration>(std::move(type), name->m_lexeme); 
    }
}


[[nodiscard]] stmnt_uptr Parser::make_declaration() {
    stmnt_uptr res = nullptr;
    if (std::optional<ast::full_type> type = peek_type()) {
        res = make_var_declaration(std::move(*type));
    } else {
        res = make_statement();
    }
    if (!res) {
        synchronize();
        return nullptr;
    }
    return res;
}

[[nodiscard]] std::unique_ptr<ast::while_stmt> Parser::make_while() {
    if (!consume(token_type::LEFT_PAREN, "expected '(' after 'while'.")) {
        return nullptr;
    }
    expr_uptr condition = make_expression();
    if (!condition) {
        return nullptr;
    }
    if (!consume(token_type::RIGHT_PAREN, "expected ')' after while-condition")) {
        return nullptr;
    }
    stmnt_uptr body = make_statement();
    if (!body) {
        return nullptr;
    }
    return std::make_unique<ast::while_stmt>(std::move(condition), std::move(body));
}

[[nodiscard]] stmnt_uptr Parser::make_for() {
    if (!consume(token_type::LEFT_PAREN, "expected '(' after 'for'.")) {
        return nullptr;
    }
    stmnt_uptr initializer = nullptr;
    if (match({token_type::SEMICOLON})) {
        initializer = nullptr;
    } else if (std::optional<ast::full_type> type = peek_type()) {
        initializer = make_var_declaration(std::move(*type));
    } else {
        initializer = make_expression_statement();
    }
    if (!initializer) {
        return nullptr;
    }
    expr_uptr condition = nullptr;
    if (!check(token_type::SEMICOLON)) {
        condition = make_expression();
    }
    if (!condition) {
        return nullptr;
    }
    if (!consume(token_type::SEMICOLON, "expected ';' after loop condition.")) {
        return nullptr;
    }
    expr_uptr increment = nullptr;
    if (!check(token_type::RIGHT_PAREN)) {
        increment = make_expression();
    }
    if (!increment) {
        return nullptr;
    }
    if (!consume(token_type::RIGHT_PAREN, "expected ')' at end of for-loop header")) {
        return nullptr;
    }
    stmnt_uptr body = make_statement();
    if (!body) {
        return nullptr;
    }
    // if (increment) {
    //     std::vector<stmnt_uptr> statements;  
    //     statements.push_back(std::move(body));
    //     statements.push_back(std::make_unique<ast::expression_stmt>(std::move(increment)));
    //     body = std::make_unique<ast::block>(std::move(statements));
    // }
    // if (initializer) {
    //     std::vector<stmnt_uptr> statements;
    //     statements.push_back(std::move(initializer));
    //     statements.push_back(std::move(body));
    //     body = std::make_unique<ast::block>(std::move(statements));
    // }

    return std::make_unique<ast::for_stmt>(std::move(initializer), std::move(condition), std::move(increment), std::move(body));
}

[[nodiscard]] stmnt_uptr Parser::make_foreach() {
    if (!consume(token_type::LEFT_PAREN, "expected '(' after 'foreach'.")) {
        return nullptr;
    }

    std::optional<ast::full_type> type = make_type();
    if (!type) {
        return nullptr;
    }
    const token* name = consume(token_type::IDENTIFIER, "expected variable name.");
    if (!name) {
        return nullptr;
    }

    if (!consume(token_type::COLON, "expected ':' after variable declaration.")) {
        return nullptr;
    }

    expr_uptr iterable = make_expression();
    if (!iterable) {
        m_errors.emplace_back(peek(), "expected iterable expression in foreach-loop.");
        return nullptr;
    }

    if (!consume(token_type::RIGHT_PAREN, "expected ')' at end of foreach-loop header")) {
        return nullptr;
    }

    stmnt_uptr body = make_statement();
    if (!body) {
        m_errors.emplace_back(peek(), "expected body statement in foreach-loop.");
        return nullptr;
    }

    return std::make_unique<ast::foreach_stmt>(ast::parameter{std::move(*type), name->m_lexeme}, std::move(iterable), std::move(body));
}

[[nodiscard]] stmnt_uptr Parser::make_statement() {
    if (match({token_type::IF})) {
        return make_if();
    } else if (match({token_type::WHILE})) {
        return make_while();
    } else if (match({token_type::FOR})) {
        return make_for();
    } else if (match({token_type::FOREACH})) {
        return make_foreach();
    } else if (match({token_type::LEFT_BRACE})) {
        return make_block();
    } else if (match({token_type::RETURN})) {
        return make_return();
    } else {
        return make_expression_statement();
    }
}

[[nodiscard]] std::unique_ptr<ast::if_stmt> Parser::make_if() {
    if (!consume(token_type::LEFT_PAREN, "expected '(' after 'if'.")) {
        return nullptr;
    }
    expr_uptr condition = make_expression();
    if (!condition) {
        return nullptr;
    }
    if (!consume(token_type::RIGHT_PAREN, "expect ')' after if-condition.")) {
        return nullptr;
    }
    stmnt_uptr then_branch = make_statement();
    if (!then_branch) {
        return nullptr;
    }
    stmnt_uptr else_branch = nullptr;
    if (match({token_type::ELSE})) {
        else_branch = make_statement();
        if (!else_branch) {
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
    if (!consume(token_type::RIGHT_BRACE, "expected '}' after block.")) {
        return nullptr;
    }
    return std::make_unique<ast::block>(std::move(statements));
}

[[nodiscard]] std::unique_ptr<ast::return_stmt> Parser::make_return() {
    if (match({token_type::SEMICOLON})) {
        return std::make_unique<ast::return_stmt>(nullptr); 
    }
    expr_uptr expression = make_expression();

    if (!consume(token_type::SEMICOLON, "expected ';' after return value.")) {
        return nullptr;
    }

    return std::make_unique<ast::return_stmt>(std::move(expression));
}

[[nodiscard]] std::unique_ptr<ast::expression_stmt> Parser::make_expression_statement() {
    expr_uptr expr = make_expression();
    if (!expr) {
        return nullptr;
    }
    if (!consume(token_type::SEMICOLON, "expected ';' after expression.")) {
        return nullptr;
    }
    return std::make_unique<ast::expression_stmt>(std::move(expr));
}

[[nodiscard]] expr_uptr Parser::make_or() {
    expr_uptr expr = make_and();
    if (!expr) {
        return nullptr;
    }
    while (match({token_type::OR, token_type::PIPE_PIPE})) {
        const token op = previous();
        expr_uptr right = make_and();
        if (!right) {
            return nullptr;
        }
        expr = std::make_unique<ast::logical_expr>(op, std::move(expr), std::move(right));
    }
    return expr;
}

[[nodiscard]] expr_uptr Parser::make_and() {
    expr_uptr expr = make_equality();
    if (!expr) {
        return nullptr;
    }
    while (match({token_type::AND, token_type::AMPERSAND_AMPERSAND})) {
        const token op = previous();
        expr_uptr right = make_equality();
        if (!right) {
            return nullptr;
        }
        expr = std::make_unique<ast::logical_expr>(op, std::move(expr), std::move(right));
    }
    return expr;
}

[[nodiscard]] expr_uptr Parser::make_assignment() {
    expr_uptr expr = make_or();
    if (!expr) {
        return nullptr;
    }
    if (match({token_type::EQUAL})) {
        const token equals = previous();
        expr_uptr value = make_assignment();
        const ast::identifier* expr_ptr = dynamic_cast<const ast::identifier*>(expr.get());
        if (expr_ptr) {
            return std::make_unique<ast::assign_expr>(std::move(expr), std::move(value));
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
    if (!expr) {
        return nullptr;
    }
    while (match({token_type::BANG_EQUAL, token_type::EQUAL_EQUAL})) {
        const token& op = previous();
        expr_uptr right = make_comparison();
        if (!right) {
            return nullptr;
        }
        switch (op.m_type) {
            case token_type::BANG_EQUAL:
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
    if (!expr) {
        return nullptr;
    }
    while (match({token_type::GREATER, token_type::GREATER_EQUAL, token_type::LESS, token_type::LESS_EQUAL})) {
        const token& op = previous();
        expr_uptr right = make_term();
        if (!right) {
            return nullptr;
        }
        switch (op.m_type) {
            case token_type::GREATER:
            case token_type::GREATER_EQUAL:
            case token_type::LESS:
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
    if (!expr) {
        return nullptr;
    }
    while (match({token_type::MINUS, token_type::PLUS})) {
        const token& op = previous();
        expr_uptr right = make_factor();
        if (!right) {
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
    if (!expr) {
        return nullptr;
    }
    while (match({token_type::SLASH, token_type::STAR})) {
        const token& op = previous();
        expr_uptr right = make_unary();
        if (!right) {
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
    if (match({token_type::BANG, token_type::PLUS, token_type::MINUS, token_type::TILDE, token_type::PLUS_PLUS, token_type::MINUS_MINUS, token_type::STAR, token_type::AMPERSAND})) {
        const token& op = previous();
        expr_uptr right = make_unary();
        if (!right) {
            return nullptr;
        }
        switch (op.m_type) {
            case token_type::BANG: {
                return std::make_unique<ast::logical_not_expr>(op, std::move(right));
            }
            case token_type::PLUS: {
                return right;
            }
            case token_type::MINUS: {
                return std::make_unique<ast::negate_expr>(op, std::move(right));
            }
            case token_type::TILDE: {
                return std::make_unique<ast::bitwise_not_expr>(op, std::move(right));
            }
            case token_type::PLUS_PLUS: {
                return std::make_unique<ast::increment_expression>(op, std::move(right));
            }
            case token_type::MINUS_MINUS: {
                //return std::make_unique<ast::decrement_expression>(op, std::move(right));
            }
            case token_type::STAR: {
                return std::make_unique<ast::dereference_expr>(op, std::move(right));
            }
            case token_type::AMPERSAND: {
                //return std::make_unique<ast::address_of_expr>(op, std::move(right));
            }
            default: {
                m_errors.emplace_back(op, "unexpected token " + op.m_lexeme);
                return nullptr;
            }
        }
    }
    return make_call();
}

[[nodiscard]] expr_uptr Parser::make_call() {
    expr_uptr expr = make_primary();

    if (!expr) {
        return nullptr;
    }

    while (true) {
        if (match({token_type::LEFT_PAREN})) {
            expr = finish_call(std::move(expr));
        } else {
            break;
        }
    }

    return expr;
}

[[nodiscard]] expr_uptr Parser::finish_call(expr_uptr&& callee) {
    std::vector<expr_uptr> args;

    if (!check(token_type::RIGHT_PAREN)) {
        do {
            args.push_back(make_expression());
        } while (match({token_type::COMMA}));
    }

    if (const token* t = consume({token_type::RIGHT_PAREN}, "expcected ')' at end of function call")) {
        return std::make_unique<ast::call_expr>(*t, std::move(callee), std::move(args));
    }

    return nullptr;
}

[[nodiscard]] expr_uptr Parser::make_match() {
    if (!consume(token_type::LEFT_PAREN, "expected '(' after match")) {
        return nullptr;
    }

    std::vector<expr_uptr> conditions;

    while (expr_uptr cond = make_expression()) {
        conditions.push_back(std::move(cond));
        if (!match({token_type::SEMICOLON})) {
            break;
        }
    }
    
    if (!consume(token_type::RIGHT_PAREN, "expected ')' after match conditions")) {
        return nullptr;
    }

    if (!consume(token_type::LEFT_BRACE, "expected '{' after match header")) {
        return nullptr;
    }
    
    std::vector<ast::match_expr::matches_t> matches;
    std::vector<expr_uptr> patterns;
    expr_uptr default_pattern = nullptr;
    bool default_pattern_reached = false;
    while (!default_pattern) {
        do {
            if (match({token_type::ELSE})) {
                default_pattern_reached = true;
            } else {
                expr_uptr literal = make_literal();
                if (!literal) {
                    m_errors.emplace_back(peek(), "expected a literal");
                    return nullptr;
                } else {
                    patterns.push_back(std::move(literal)); 
                }
            }
        } while (match({token_type::COMMA}));

        if (!consume({token_type::ARROW}, "expected '->' after pattern list")) {
            return nullptr;
        }

        expr_uptr matched_expression = make_expression();
        if (!matched_expression) {
            m_errors.emplace_back(peek(), "expected expression after '->'");
        }

        if (!default_pattern_reached && !consume({token_type::COMMA}, "expected ',' at end of pattern-match expression")) {
            return nullptr;
        }
        
        if (default_pattern_reached) {
            default_pattern = std::move(matched_expression);
        } else {
            matches.emplace_back(std::move(patterns), std::move(matched_expression));
        }
        patterns.clear();
    }

    if (!consume(token_type::RIGHT_BRACE, "expected '}' after match patterns")) {
        return nullptr;
    }
    
    return std::make_unique<ast::match_expr>(std::move(conditions), std::move(matches), std::move(default_pattern));
}

[[nodiscard]] expr_uptr Parser::make_literal() {
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
        const sid64 val = std::get<sid64>(previous().m_literal);
        const sid64_literal sid = { val, previous().m_lexeme };
        return std::make_unique<ast::literal>(sid);
    }
    return nullptr;
}

[[nodiscard]] expr_uptr Parser::make_primary() {
    if (expr_uptr literal = make_literal()) {
        return literal;
    } else if (match({token_type::IDENTIFIER})) {
        return std::make_unique<ast::identifier>(previous());
    } else if (match({token_type::MATCH})) {
        return make_match();
    } else if (match({token_type::LEFT_PAREN})) {
        std::unique_ptr expr = make_expression();
        if(!consume(token_type::RIGHT_PAREN, "expected ')' after expression")) {
            return nullptr;
        }
        return std::make_unique<ast::grouping>(std::move(expr));
    }

    m_errors.emplace_back(previous(), "expected expression after '" + previous().m_lexeme + "'");
    return nullptr;
}


}