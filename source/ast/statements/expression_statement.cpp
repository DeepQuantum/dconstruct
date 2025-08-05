#include "expression_statement.h"

namespace dconstruct::ast {

void expression_stmnt::pseudo(std::ostream& os) const {
    os << m_expression << ';';
}

void expression_stmnt::ast(std::ostream& os) const {
    os << "expression_statement[" << m_expression << ']';
}

[[nodiscard]] b8 expression_stmnt::equals(const statement& rhs) const noexcept {
    const expression_stmnt* rhs_ptr = dynamic_cast<const expression_stmnt*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return *m_expression == *rhs_ptr->m_expression;
}

}