#include "ast/primary_expressions/grouping.h"

namespace dconstruct::ast {

void grouping::ast(std::ostream& os) const {
    os << "grouping[" << *m_expr << ']';
}

void grouping::pseudo(std::ostream& os) const {
    os << '(' << *m_expr << ')';
}

[[nodiscard]] b8 grouping::equals(const expression &rhs) const noexcept {
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
    return std::make_unique<grouping>(m_expr != nullptr ? m_expr->clone() : nullptr);
}

[[nodiscard]] std::optional<full_type> grouping::compute_type(const compiler::environment& env) const {
    return m_expr->compute_type(env);
}
}