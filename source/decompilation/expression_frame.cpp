#include "expression_frame.h"
#include "assign_statement.h"

namespace dconstruct::dcompiler {

void expression_frame::move(const u32 dst, const u32 src) {
    std::unique_ptr<ast::identifier> id = std::make_unique<ast::identifier>(get_next_var_idx());
    std::unique_ptr<ast::expression> rhs = m_typedExpressions[src].m_expr->eval();
    const expression_type src_type = m_typedExpressions[src].m_type;
    std::unique_ptr<ast::assign_expr> assign = std::make_unique<ast::assign_expr>(std::move(id), std::move(rhs));
    m_statements.emplace_back(std::move(std::make_unique<ast::assign_statement>(assign.get())));
    m_typedExpressions[dst] = { std::move(assign), src_type };
}

}