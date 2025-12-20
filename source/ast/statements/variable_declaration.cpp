#include "ast/statements/variable_declaration.h"

namespace dconstruct::ast {

void variable_declaration::pseudo_c(std::ostream& os) const {
    os << type_to_declaration_string(m_type) << ' ' << m_identifier;
    if (m_init != nullptr) {
        os << " = " << *m_init;
    }
    os << ';';
}

void variable_declaration::pseudo_py(std::ostream& os) const {
    os << m_identifier;
    if (m_init != nullptr) {
        os << " = " << *m_init;
    }
}

void variable_declaration::pseudo_racket(std::ostream& os) const {
    os << "(define " << m_identifier;
    if (m_init != nullptr) {
        os << ' ' << *m_init;
    }
    os << ')';
}

[[nodiscard]] bool variable_declaration::equals(const statement& rhs) const noexcept {
    const variable_declaration* rhs_ptr = dynamic_cast<const variable_declaration*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_type.index() == rhs_ptr->m_type.index() && m_identifier == rhs_ptr->m_identifier && m_init == rhs_ptr->m_init;
}

void variable_declaration::decomp_optimization_pass(second_pass_env& env) noexcept {
    assert(!env.lookup(m_identifier));
    env.define(m_identifier, {&m_init, 0});
}

}