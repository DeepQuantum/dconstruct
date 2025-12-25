#include "ast/statements/while.h"


namespace dconstruct::ast {

void while_stmt::pseudo_c(std::ostream& os) const {
    os << "while (" << *m_condition << ") " << *m_body;
}


void while_stmt::pseudo_py(std::ostream& os) const {
    os << "while " << *m_condition << ":\n" << *m_body;
}

void while_stmt::pseudo_racket(std::ostream& os) const {
    os << "(while " << *m_condition << " " << *m_body << ")";
}



[[nodiscard]] bool while_stmt::equals(const statement& rhs) const noexcept {
    const while_stmt* rhs_ptr = dynamic_cast<const while_stmt*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_condition == rhs_ptr->m_condition && m_body == rhs_ptr->m_body;
}


[[nodiscard]] std::unique_ptr<statement> while_stmt::clone() const noexcept {
    return std::make_unique<while_stmt>(m_condition->clone(), m_body->clone());
}


VAR_OPTIMIZATION_ACTION while_stmt::var_optimization_pass(var_optimization_env& env)  noexcept {
    expression::check_var_optimization(&m_condition, env);
    statement::check_var_optimization(&m_body, env);
    return VAR_OPTIMIZATION_ACTION::NONE;
}


FOREACH_OPTIMIZATION_ACTION while_stmt::foreach_optimization_pass(foreach_optimization_env& env)  noexcept {
    if (auto action = m_condition->foreach_optimization_pass(env); action != FOREACH_OPTIMIZATION_ACTION::NONE) {
        return action;
    }
    statement::check_foreach_optimization(&m_body, env);
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

}