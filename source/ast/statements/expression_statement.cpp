#include "ast/statements/expression_statement.h"

namespace dconstruct::ast {

void expression_stmt::pseudo_c(std::ostream& os) const {
    os << *m_expression << ';';
}

void expression_stmt::pseudo_py(std::ostream& os) const {
    os << *m_expression << '\n';
}

void expression_stmt::pseudo_racket(std::ostream& os) const {
    os << *m_expression;
}

[[nodiscard]] bool expression_stmt::equals(const statement& rhs) const noexcept {
    const expression_stmt* rhs_ptr = dynamic_cast<const expression_stmt*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return *m_expression == *rhs_ptr->m_expression;
}

[[nodisarc]] std::unique_ptr<statement> expression_stmt::clone() const noexcept {
    return std::make_unique<expression_stmt>(m_expression->clone()); 
}

OPTIMIZATION_ACTION expression_stmt::decomp_optimization_pass(optimization_pass_context& optimization_ctx) noexcept {
    const OPTIMIZATION_ACTION action = expression::check_optimization(&m_expression, optimization_ctx);
    if (action == OPTIMIZATION_ACTION::BEGIN_FOREACH || action == OPTIMIZATION_ACTION::END_FOREACH) {
        return action;
    }
    return OPTIMIZATION_ACTION::NONE;
}

}