#include "ast/binary_expressions/assign.h"

namespace dconstruct::ast {    

void assign_expr::pseudo(std::ostream& os) const {
    os << m_lhs.m_lexeme << " = " << *m_rhs;
}


void assign_expr::ast(std::ostream& os) const {
    os << "assign[" << m_lhs.m_lexeme << ", " << *m_rhs << ']';
}

[[nodiscard]] expr_uptr assign_expr::simplify() const {
    return m_rhs->simplify();
}

[[nodiscard]] std::optional<ast::full_type> assign_expr::compute_type(const compiler::environment& env) const {
    return m_rhs->compute_type(env);
}


[[nodiscard]] b8 assign_expr::equals(const expression& rhs) const noexcept {
    const assign_expr* rhs_ptr = dynamic_cast<const assign_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_lhs == rhs_ptr->m_lhs && m_rhs == rhs_ptr->m_rhs;
}

[[nodiscard]] expr_uptr assign_expr::clone() const {
    return std::make_unique<assign_expr>(m_lhs, m_rhs != nullptr ? m_rhs->clone() : nullptr);
}

}