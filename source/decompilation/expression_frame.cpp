#include "decompilation/expression_frame.h"

namespace dconstruct::dcompiler {

void expression_frame::move(const u32 dst, const u32 src) {
    auto id = std::make_unique<ast::identifier>(get_next_var_idx());
    auto rhs = m_transformableExpressions[src].m_expr->eval();
    const ast::type_kind src_type = m_transformableExpressions[src].m_type;
    auto assign = std::make_unique<ast::assign_expr>(std::move(id), std::move(rhs));
    m_statements.emplace_back(std::make_unique<ast::expression_stmt>(assign->eval()));
    m_transformableExpressions[dst] = { std::move(assign), src_type };
}

void expression_frame::load_literal(const u8 dst, const ast::primitive_value_type& value) {
    auto literal = std::make_unique<ast::literal>(value);
    auto id = std::make_unique<ast::identifier>(get_next_var_idx());
    auto assign = std::make_unique<ast::assign_expr>(std::move(id), std::move(literal));
    m_statements.emplace_back(std::make_unique<ast::expression_stmt>(assign->eval()));
    m_transformableExpressions[dst] = { std::move(assign), ast::kind_from_primitive_value(value) };
}

}