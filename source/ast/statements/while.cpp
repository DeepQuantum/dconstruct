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

bool while_stmt::decomp_optimization_pass(second_pass_env& env) noexcept {
    if (m_condition->decomp_optimization_pass(env)) {
        env->lookup(static_cast<identifier&>(*m_condition).m_name.m_lexeme)->m_firstUsageSite = &m_condition;
    }
    m_body->decomp_optimization_pass(env);
    return false;
}

}