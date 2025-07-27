#include "statements.h"

namespace dconstruct::ast {

void assign_statement::pseudo(std::ostream& os) const noexcept {
    os << *m_expr << ';';
}

void assign_statement::ast(std::ostream& os) const noexcept {
    os << "assign_stmt[" << *m_expr << ']';
}

[[nodiscard]] b8 assign_statement::equals(const statement& rhs) const noexcept {
    const assign_statement* rhs_ptr = dynamic_cast<const assign_statement*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return *m_expr == *rhs_ptr->m_expr;
}

}