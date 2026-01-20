#include "ast/primary_expressions/grouping.h"
#include "ast/primary_expressions/identifier.h"


namespace dconstruct::ast {

    
void grouping::pseudo_c(std::ostream& os) const {
    os << '(' << *m_expr << ')';
}

void grouping::pseudo_py(std::ostream& os) const {
    os << '(' << *m_expr << ')';
}

void grouping::pseudo_racket(std::ostream& os) const {
    os << *m_expr;
}

[[nodiscard]] bool grouping::equals(const expression &rhs) const noexcept {
    const grouping* rhs_ptr = dynamic_cast<const grouping*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_expr == rhs_ptr->m_expr;
}

[[nodiscard]] expr_uptr grouping::simplify() const {
    return m_expr->simplify();
}

[[nodiscard]] expr_uptr grouping::clone() const {
    auto expr = std::make_unique<grouping>(m_expr != nullptr ? m_expr->clone() : nullptr);
    if (m_type) expr->set_type(*m_type);
    return expr;
}

[[nodiscard]] full_type grouping::compute_type_unchecked(const compiler::scope& env) const noexcept {
    return m_expr->compute_type_unchecked(env);
}

[[nodiscard]] semantic_check_res grouping::compute_type_checked(compiler::scope& env) const noexcept {
    return m_expr->get_type_checked(env);
}

[[nodiscard]] u16 grouping::calc_complexity() const noexcept {
    return 1 + m_expr->get_complexity();
}

VAR_OPTIMIZATION_ACTION grouping::var_optimization_pass(var_optimization_env& env) noexcept {
    env.check_action(&m_expr);
    return VAR_OPTIMIZATION_ACTION::NONE;
}


FOREACH_OPTIMIZATION_ACTION grouping::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    env.check_action(&m_expr);
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

}