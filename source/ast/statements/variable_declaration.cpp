#include "ast/statements/variable_declaration.h"

namespace dconstruct::ast {

void variable_declaration::pseudo(std::ostream& os) const {
    os << m_typeName << ' ' << m_identifier;
    if (m_init != nullptr) {
        os << " = " << *m_init;
    }
    os << ';';
}

void variable_declaration::ast(std::ostream& os) const {
    os << "variable_declaration[" << m_typeName << ", " << m_identifier << ", ";
    if (m_init != nullptr) {
        os << '{' << *m_init << '}';
    } else {
        os << "null";
    }
    os << ';';
}

[[nodiscard]] b8 variable_declaration::equals(const statement& rhs) const noexcept {
    const variable_declaration* rhs_ptr = dynamic_cast<const variable_declaration*>(&rhs); 
    if (rhs_ptr == nullptr) { 
        return false; 
    }
    return m_typeName == rhs_ptr->m_typeName && m_identifier == rhs_ptr->m_identifier && *m_init == *rhs_ptr->m_init;
}

}