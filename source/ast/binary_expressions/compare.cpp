#include "compare.h"

namespace dconstruct::ast {
[[nodiscard]] b8 compare_expr::equals(const expression& rhs) const noexcept {
    const compare_expr* rhs_ptr = dynamic_cast<const compare_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_lhs == rhs_ptr->m_lhs && m_rhs == rhs_ptr->m_rhs && m_compType == rhs_ptr->m_compType;
}


[[nodiscard]] std::unique_ptr<expression> compare_expr::eval() const {
    return nullptr;
}
}