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

[[nodiscard]] std::vector<semantic_check_error> variable_declaration::check_semantics(compilation::scope& scope) const noexcept {
    if (m_init) {
        const semantic_check_res init_type = m_init->get_type_checked(scope);
        if (!init_type) {
            return {init_type.error()};
        }

        const std::optional<std::string> assign_err = not_assignable_reason(m_type, *init_type);

        if (assign_err) {
            return {semantic_check_error{*assign_err}};
        }
    }
    scope.define(m_identifier, m_type);

    return {};
}

[[nodiscard]] emission_err variable_declaration::emit_dc(compilation::function& fn, compilation::global_state& global) const noexcept {
    const emission_res new_var_reg = fn.get_next_unused_register();
    if (!new_var_reg) {
        return new_var_reg.error();
    }
    assert(!fn.m_varsToRegs.lookup(m_identifier));
    fn.m_varsToRegs.define(m_identifier, *new_var_reg);

    if (m_init) {
        const emission_res init_emit = m_init->emit_dc(fn, global, *new_var_reg);
        if (!init_emit) {
            return init_emit.error();
        }
    }
    return std::nullopt;
}

VAR_OPTIMIZATION_ACTION variable_declaration::var_optimization_pass(var_optimization_env& env)  noexcept {
    if (m_init) {
        env.check_action(&m_init);
    }
    if (!m_identifier.starts_with("var") || env.m_env.m_values.contains(m_identifier)) {
        return VAR_OPTIMIZATION_ACTION::NONE;
    }
    env.m_env.define(m_identifier, {});
    return VAR_OPTIMIZATION_ACTION::VAR_DECLARATION;
}

FOREACH_OPTIMIZATION_ACTION variable_declaration::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    if (m_init) {
        return m_init->foreach_optimization_pass(env);
    }
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

MATCH_OPTIMIZATION_ACTION variable_declaration::match_optimization_pass(match_optimization_env& env) noexcept {
    return !m_init ? MATCH_OPTIMIZATION_ACTION::RESULT_VAR_DECLARATION : MATCH_OPTIMIZATION_ACTION::NONE;
}

}