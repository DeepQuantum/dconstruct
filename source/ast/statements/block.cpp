#include "ast/statements/block.h"

namespace dconstruct::ast {

void block::pseudo(std::ostream& os) const {
    if (m_statements.empty()) {
        os << "{}" << '\n';
    } else {
        os << "{\n";
        os << indent_more;
        for (const auto& stmnt : m_statements) {
            os << indent << *stmnt << '\n';
        } 
        os << indent_less;
        os << indent << '}';
    }
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