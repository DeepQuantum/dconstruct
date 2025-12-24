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


OPTIMIZATION_ACTION while_stmt::decomp_optimization_pass(optimization_pass_context& optimization_ctx) noexcept {
    expression::check_optimization(&m_condition, optimization_ctx);
    statement::check_optimization(&m_body, optimization_ctx);
    return OPTIMIZATION_ACTION::NONE;
}

}