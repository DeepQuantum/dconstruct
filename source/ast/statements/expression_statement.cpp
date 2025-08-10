#include "ast/statements/expression_statement.h"

namespace dconstruct::ast {

void expression_stmt::pseudo(std::ostream& os) const {
    os << *m_expression << ';';
}

void expression_stmt::ast(std::ostream& os) const {
    os << "expression_statement[" << *m_expression << ']';
}

[[nodiscard]] b8 expression_stmt::equals(const statement& rhs) const noexcept {
    const expression_stmt* rhs_ptr = dynamic_cast<const expression_stmt*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return *m_expression == *rhs_ptr->m_expression;
}

}