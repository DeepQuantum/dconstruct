#include "div.h"


namespace dconstruct::ast {
    
[[nodiscard]] b8 div_expr::equals(const expression& rhs) const noexcept {
    const div_expr* rhs_ptr = dynamic_cast<const div_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_lhs == rhs_ptr->m_lhs && m_rhs == rhs_ptr->m_rhs;
}

[[nodiscard]] std::unique_ptr<expression> div_expr::eval() const { 
    return nullptr;
}

}
