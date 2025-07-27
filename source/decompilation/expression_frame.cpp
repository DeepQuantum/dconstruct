#include "expression_frame.h"

namespace dconstruct::dcompiler {

void expression_frame::move(const u32 dst, const u32 src) noexcept {
    std::unique_ptr<ast::identifier> id = std::make_unique<ast::identifier>(get_next_var_idx());
    std::unique_ptr<ast::expression> rhs = m_expressions[src]->eval();
    std::unique_ptr<ast::assign_expr> assign = std::make_unique<ast::assign_expr>(std::move(id), std::move(rhs));
    m_statements.emplace_back(std::move(std::make_unique<ast::assign_statement>(assign.get())));
    m_expressions[dst] = std::move(assign);
}

}