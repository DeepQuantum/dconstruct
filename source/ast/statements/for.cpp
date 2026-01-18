#include "ast/statements/variable_declaration.h"
#include "ast/statements/for.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/foreach_optimization.h"

namespace dconstruct::ast {

void for_stmt::pseudo_c(std::ostream& os) const {
    os << "for (" << *m_init << ' ' << *m_condition << "; " << *m_incr << ") " << *m_body;
}


void for_stmt::pseudo_py(std::ostream& os) const {
    const auto& var = static_cast<const variable_declaration&>(*m_init).m_identifier;
    const auto& range = static_cast<const binary_expr&>(*m_condition).m_rhs;
    os << "for " << var << " in range(" << *range << "):\n" << *m_body;
}

void for_stmt::pseudo_racket(std::ostream& os) const {
    const auto& var = static_cast<variable_declaration&>(*m_init).m_identifier;
    const auto& range = static_cast<binary_expr&>(*m_condition).m_rhs;
    os << "(for ([" << var << " (in-range " << *range << ")])\n" << *m_body << ')';
}

[[nodiscard]] bool for_stmt::equals(const statement& rhs) const noexcept {
    const for_stmt* rhs_ptr = dynamic_cast<const for_stmt*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_init == rhs_ptr->m_init && m_condition == rhs_ptr->m_condition && m_incr == rhs_ptr->m_incr && m_body == rhs_ptr->m_body;
}

[[nodiscard]] std::unique_ptr<statement> for_stmt::clone() const noexcept {
    return std::make_unique<for_stmt>(m_init->clone(), m_condition->clone(), m_incr->clone(), m_body->clone());
}


VAR_OPTIMIZATION_ACTION for_stmt::var_optimization_pass(var_optimization_env& env)  noexcept {
    env.check_action(&m_init);
    env.check_action(&m_condition);
    env.check_action(&m_incr);
    env.check_action(&m_body);

    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION for_stmt::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    env.check_action(&m_condition);
    env.check_action(&m_body);
    return FOREACH_OPTIMIZATION_ACTION::FOR;
}

MATCH_OPTIMIZATION_ACTION for_stmt::match_optimization_pass(match_optimization_env& env) noexcept {
    return m_body->match_optimization_pass(env);
}

}