#include "ast/statements/return.h"

namespace dconstruct::ast {

void return_stmt::pseudo_c(std::ostream& os) const {
    os << "return " << *m_expr << ';';
}

void return_stmt::pseudo_py(std::ostream& os) const {
    os << "return " << *m_expr << '\n';
}

[[nodiscard]] bool return_stmt::equals(const statement& rhs) const noexcept {
    const return_stmt* rhs_ptr = dynamic_cast<const return_stmt*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_expr == rhs_ptr->m_expr;
}

}