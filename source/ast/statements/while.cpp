#include "ast/statements/while.h"


namespace dconstruct::ast {

void while_stmt::pseudo(std::ostream& os) const {
    os << "while (" << *m_condition << ") " << *m_body;
}


void while_stmt::ast(std::ostream& os) const {
    os << "while[condition=" << *m_condition << ", body=" << *m_body << "]";
}

[[nodiscard]] b8 while_stmt::equals(const statement& rhs) const noexcept {
    const while_stmt* rhs_ptr = dynamic_cast<const while_stmt*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_condition == rhs_ptr->m_condition && m_body == rhs_ptr->m_body;
}

}