#include "ast/statements/for.h"


namespace dconstruct::ast {

void for_stmt::pseudo_c(std::ostream& os) const {
    os << "for (" << *m_init << ' ' << *m_condition << "; " << *m_incr << ") " << *m_body;
}


void for_stmt::pseudo_py(std::ostream& os) const {
    //os << "for " << m_init-> << 
}

[[nodiscard]] bool for_stmt::equals(const statement& rhs) const noexcept {
    const for_stmt* rhs_ptr = dynamic_cast<const for_stmt*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_init == rhs_ptr->m_init && m_condition == rhs_ptr->m_condition && m_incr == rhs_ptr->m_incr && m_body == rhs_ptr->m_body;
}

}