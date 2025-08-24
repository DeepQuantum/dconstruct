#include "decompilation/expression_frame.h"

namespace dconstruct::dcompiler {

expr_uptr& expression_frame::move(const u32 dst, const u32 src) {
    return m_transformableExpressions[dst] = m_transformableExpressions[src]->clone();
}

expr_uptr& expression_frame::load_expression_into_var(const u32 dst, expr_uptr&& expr) {
    auto id = std::make_unique<ast::identifier>(compiler::token{ compiler::token_type::IDENTIFIER, get_next_var() });
    const std::string name = id->m_name.m_lexeme;
    const ast::full_type type = id->compute_type(m_env).value_or(ast::primitive_type{ ast::primitive_kind::UNKNOWN });
    const std::string type_name = type_to_declaration_string(type);

    auto var_declaration = std::make_unique<ast::variable_declaration>(type_name, name, std::move(expr));

    m_statements.push_back(std::move(var_declaration));
    return m_transformableExpressions[dst] = std::move(id);
}

expr_uptr& expression_frame::load_literal(const u8 dst, const ast::primitive_value& value) {
    return m_transformableExpressions[dst] = std::make_unique<ast::literal>(value);
}

expr_uptr& expression_frame::call(const Instruction& istr) {
    expr_uptr callee = m_transformableExpressions[istr.destination]->clone();  
    std::vector<expr_uptr> args;

    for (u8 i = 0; i < istr.operand2; ++i) {
        args.push_back(m_transformableExpressions[49 + i]->clone()->simplify());
    }

    return m_transformableExpressions[istr.destination] = std::make_unique<ast::call_expr>(compiler::token{compiler::token_type::_EOF, ""}, std::move(callee), std::move(args));
}

}