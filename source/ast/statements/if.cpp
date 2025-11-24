#include "ast/statements/if.h"
#include "ast/statements/block.h"

namespace dconstruct::ast {

void if_stmt::pseudo_c(std::ostream& os) const {
    os << "if (" << *m_condition << ") " << *m_then;
    if (m_else != nullptr) {
        os << " else " << *m_else;
    }
}

void if_stmt::pseudo_py(std::ostream& os) const {
    os << "if " << *m_condition << ' ' << *m_then;
    if (m_else != nullptr) {
        os << " else " << *m_else;
    }
}

void if_stmt::pseudo_racket(std::ostream& os) const {
    os << "(if "<< *m_condition << " " << *m_then << " ";
    if (m_else != nullptr) {
        os << *m_else;
    } else {
        os << "(void)";
    }
    os << ")";
}

[[nodiscard]] bool if_stmt::equals(const statement& rhs) const noexcept {
    const if_stmt* rhs_ptr = dynamic_cast<const if_stmt*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_condition == rhs_ptr->m_condition && m_then == rhs_ptr->m_then && m_else == rhs_ptr->m_then;
}

}