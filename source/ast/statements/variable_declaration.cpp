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

void statement::check_var_optimization(stmnt_uptr* statement, var_optimization_env& env) {
    const auto pass_action = statement->get()->var_optimization_pass(env);
    switch (pass_action) {
        case VAR_OPTIMIZATION_ACTION::VAR_DECLARATION: {
            env.lookup(static_cast<variable_declaration&>(**statement).m_identifier)->m_declaration = statement;
            break;
        }
        default: {
            break;
        }
    }
}

void statement::check_foreach_optimization(stmnt_uptr* statement, foreach_optimization_env& env) {
    const auto pass_action = statement->get()->foreach_optimization_pass(env);
    switch (pass_action) {
        case FOREACH_OPTIMIZATION_ACTION::BEGIN_FOREACH: {
            env = statement;
            break;
        }
        case FOREACH_OPTIMIZATION_ACTION::END_FOREACH: {
            *env = nullptr;
            *statement = nullptr;
            break;
        }
    }
}

VAR_OPTIMIZATION_ACTION variable_declaration::var_optimization_pass(var_optimization_env& env)  noexcept {
    if (m_init) {
        expression::check_var_optimization(&m_init, env);
    }
    if (!m_identifier.starts_with("var") || env.m_values.contains(m_identifier)) {
        return VAR_OPTIMIZATION_ACTION::NONE;
    }
    env.define(m_identifier, {});
    return VAR_OPTIMIZATION_ACTION::VAR_DECLARATION;
}

FOREACH_OPTIMIZATION_ACTION variable_declaration::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    if (m_init) {
        return m_init->foreach_optimization_pass(env);
    }
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

}