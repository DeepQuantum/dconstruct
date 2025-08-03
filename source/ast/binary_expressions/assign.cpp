#include "assign.h"

namespace dconstruct::ast {    
[[nodiscard]] b8 assign_expr::equals(const expression& rhs) const noexcept {
    const assign_expr* rhs_ptr = dynamic_cast<const assign_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_lhs == rhs_ptr->m_lhs && m_rhs == rhs_ptr->m_rhs;
}

[[nodiscard]] std::unique_ptr<expression> assign_expr::eval() const {
    return m_rhs->eval();
}
}