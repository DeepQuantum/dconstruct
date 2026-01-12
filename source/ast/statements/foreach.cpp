#include "ast/statements/variable_declaration.h"
#include "ast/statements/foreach.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/foreach_optimization.h"

namespace dconstruct::ast {


void foreach_stmt::pseudo_c(std::ostream& os) const {
    os << "foreach (" << m_var  << " : " << *m_iterable << ") " << *m_body;
}


void foreach_stmt::pseudo_py(std::ostream& os) const {
    return pseudo_c(os);
}

void foreach_stmt::pseudo_racket(std::ostream& os) const {
    return pseudo_c(os);
}

[[nodiscard]] bool foreach_stmt::equals(const statement& rhs) const noexcept {
    const foreach_stmt* rhs_ptr = dynamic_cast<const foreach_stmt*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_var == rhs_ptr->m_var && m_iterable == rhs_ptr->m_iterable && m_body == rhs_ptr->m_body;
}

[[nodiscard]] std::unique_ptr<statement> foreach_stmt::clone() const noexcept {
    return std::make_unique<foreach_stmt>(m_var, m_iterable->clone(), m_body->clone());
}


VAR_OPTIMIZATION_ACTION foreach_stmt::var_optimization_pass(var_optimization_env& env) noexcept {
    env.check_action(&m_iterable);
    env.check_action(&m_body);
    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION foreach_stmt::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

MATCH_OPTIMIZATION_ACTION foreach_stmt::match_optimization_pass(match_optimization_env& env) noexcept {
    return m_body->match_optimization_pass(env);
}

}