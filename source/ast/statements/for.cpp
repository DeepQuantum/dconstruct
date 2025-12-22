#include "ast/statements/variable_declaration.h"
#include "ast/statements/for.h"


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


VAR_FOLDING_ACTION for_stmt::decomp_optimization_pass(second_pass_env& env) noexcept {
    statement::check_optimization(&m_init, env);
    expression::check_optimization(&m_condition, env);
    expression::check_optimization(&m_incr, env);
    statement::check_optimization(&m_body, env);
    return VAR_FOLDING_ACTION::NONE;
}

}