#include "ast/binary_expressions/assign.h"

namespace dconstruct::ast {    

void assign_expr::pseudo_c(std::ostream& os) const {
    os << *m_lhs << " = " << *m_rhs;
}


void assign_expr::pseudo_py(std::ostream& os) const {
    os << *m_lhs << " = " << *m_rhs;
}

void assign_expr::pseudo_racket(std::ostream& os) const {
    os << "(set! " << *m_lhs << ' ' << *m_rhs << ')';
}

[[nodiscard]] expr_uptr assign_expr::simplify() const {
    return m_rhs->simplify();
}

[[nodiscard]] ast::full_type assign_expr::compute_type(const type_environment& env) const {
    return m_rhs->compute_type(env);
}


[[nodiscard]] bool assign_expr::equals(const expression& rhs) const noexcept {
    const assign_expr* rhs_ptr = dynamic_cast<const assign_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_lhs == rhs_ptr->m_lhs && m_rhs == rhs_ptr->m_rhs;
}

[[nodiscard]] expr_uptr assign_expr::clone() const {
    return std::make_unique<assign_expr>(m_lhs->clone(), m_rhs != nullptr ? m_rhs->clone() : nullptr);
}

[[nodiscard]] inline u16 assign_expr::complexity() const noexcept {
    return 1 + m_rhs->complexity();
}

}