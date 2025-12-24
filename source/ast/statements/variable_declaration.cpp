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

OPTIMIZATION_ACTION statement::check_optimization(std::unique_ptr<ast::statement>* statement, optimization_pass_context& optimization_ctx) {
    const auto pass_action = statement->get()->decomp_optimization_pass(optimization_ctx);
    switch (pass_action) {
        case OPTIMIZATION_ACTION::VAR_DECLARATION: {
            optimization_ctx.m_variables.lookup(static_cast<variable_declaration&>(**statement).m_identifier)->m_declaration = statement;
            break;
        }
        case OPTIMIZATION_ACTION::BEGIN_FOREACH: {
            optimization_ctx.m_foreachBegin = statement;
        }
        case OPTIMIZATION_ACTION::END_FOREACH: {
            optimization_ctx.m_foreachEnd = statement;
        }
        default: {
            break;
        }
    }
    return OPTIMIZATION_ACTION::NONE;
}

OPTIMIZATION_ACTION variable_declaration::decomp_optimization_pass(optimization_pass_context& optimization_ctx) noexcept {
    if (m_init) {
        expression::check_optimization(&m_init, optimization_ctx);
    }
    if (!m_identifier.starts_with("var") || optimization_ctx.m_variables.m_values.contains(m_identifier)) {
        return OPTIMIZATION_ACTION::NONE;
    }
    optimization_ctx.m_variables.define(m_identifier, {});
    return OPTIMIZATION_ACTION::VAR_DECLARATION;
}

}