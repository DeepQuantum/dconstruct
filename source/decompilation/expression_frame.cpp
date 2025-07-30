#include "expression_frame.h"
#include "assign_statement.h"

namespace dconstruct::dcompiler {

void expression_frame::move(const u32 dst, const u32 src) {
    std::unique_ptr<ast::identifier> id = std::make_unique<ast::identifier>(get_next_var_idx());
    std::unique_ptr<ast::expression> rhs = m_typedExpressions[src].m_expr->eval();
    const ast::type_kind src_type = m_typedExpressions[src].m_type;
    std::unique_ptr<ast::assign_expr> assign = std::make_unique<ast::assign_expr>(std::move(id), std::move(rhs));
    m_statements.emplace_back(std::move(std::make_unique<ast::assign_statement>(assign.get())));
    m_typedExpressions[dst] = { std::move(assign), src_type };
}

void expression_frame::load_literal(const u8 dst, const ast::primitive_value_type& value) {
    std::unique_ptr<ast::literal> literal = std::make_unique<ast::literal>(value);
    std::unique_ptr<ast::identifier> id = std::make_unique<ast::identifier>(get_next_var_idx());
    std::unique_ptr<ast::assign_expr> assign = std::make_unique<ast::assign_expr>(std::move(id), std::move(literal));
    m_statements.emplace_back(std::make_unique<ast::assign_statement>(assign.get()));
    m_typedExpressions[dst] = { std::move(assign), ast::kind_from_primitive_value(value) };
}

}