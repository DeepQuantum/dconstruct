#include "ast/primary_expressions/grouping.h"

namespace dconstruct::ast {

    
void grouping::pseudo_c(std::ostream& os) const {
    os << '(' << *m_expr << ')';
}

void grouping::pseudo_py(std::ostream& os) const {
    os << '(' << *m_expr << ')';
}

void grouping::pseudo_racket(std::ostream& os) const {
    os << *m_expr;
}

[[nodiscard]] bool grouping::equals(const expression &rhs) const noexcept {
    const grouping* rhs_ptr = dynamic_cast<const grouping*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_expr == rhs_ptr->m_expr;
}

[[nodiscard]] expr_uptr grouping::simplify() const {
    return m_expr->simplify();
}

[[nodiscard]] expr_uptr grouping::clone() const {
    auto expr = std::make_unique<grouping>(m_expr != nullptr ? m_expr->clone() : nullptr);
    if (!is_unknown(m_type)) expr->set_type(m_type);
    return expr;
}

[[nodiscard]] full_type grouping::compute_type(const compiler::environment& env) const {
    return m_expr->compute_type(env);
}

[[nodiscard]] u16 grouping::complexity() const noexcept {
    return m_expr->complexity();
}
}