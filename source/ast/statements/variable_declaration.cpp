#include "ast/statements/variable_declaration.h"

#include <iostream>

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

[[nodiscard]] std::unique_ptr<statement> variable_declaration::clone() const noexcept {
    return std::make_unique<variable_declaration>(m_type, m_identifier, m_init ? m_init->clone() : nullptr);
}

void statement::check_optimization(std::unique_ptr<ast::statement>* statement, second_pass_env& env) {
    if (statement->get()->decomp_optimization_pass(env)) {
        env.lookup(static_cast<variable_declaration&>(*statement->get()).m_identifier)->m_original = statement;
    }
}



bool variable_declaration::decomp_optimization_pass(second_pass_env& env) noexcept {
    if (m_init) {
        expression::check_optimization(&m_init, env);
    }
    if (!m_identifier.starts_with("var") || env.m_values.contains(m_identifier)) {
        return false;
    }
    env.define(m_identifier, {});
    return true;
}

}