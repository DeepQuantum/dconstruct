#include "ast/statements/block.h"

namespace dconstruct::ast {

void block::pseudo(std::ostream& os) const {
    os << "{";
    for (const auto& stmnt : m_statements) {
        os << *stmnt << '\n';
    } 
    os << '}';
}


void block::ast(std::ostream& os) const {
    os << "block[";
    for (const auto& stmnt : m_statements) {
        os << *stmnt << ',';
    } 
    os << ']';
}

[[nodiscard]] b8 block::equals(const statement& rhs) const noexcept {
    const block* rhs_ptr = dynamic_cast<const block*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_statements == rhs_ptr->m_statements;
}

}