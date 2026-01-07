#include "ast/primary_expressions/ternary.h"


namespace dconstruct::ast {


void ternary_expr::pseudo_c(std::ostream& os) const {
    os << "if " << *m_condition << " then " << *m_then << " else " << *m_else;
}

void ternary_expr::pseudo_py(std::ostream& os) const {
    os << "if " << *m_condition << " then " << *m_then << " else " << *m_else;
}
void ternary_expr::pseudo_racket(std::ostream& os) const {
    os << "if " << *m_condition << " then " << *m_then << " else " << *m_else;
}

[[nodiscard]] expr_uptr ternary_expr::simplify() const {
    return std::make_unique<ternary_expr>(m_condition->simplify(), m_then->simplify(), m_else->simplify());
}

[[nodiscard]] bool ternary_expr::equals(const expression& rhs) const noexcept {
    const ternary_expr* rhs_ptr = dynamic_cast<const ternary_expr*>(&rhs);
    if (!rhs_ptr) {
        return false;
    }
    return m_condition == rhs_ptr->m_condition && m_then == rhs_ptr->m_then && m_else == rhs_ptr->m_else;
}

[[nodiscard]] u16 ternary_expr::calc_complexity() const noexcept {
    return 1 + m_condition->get_complexity() + m_then->get_complexity() + m_else->get_complexity();
}

[[nodiscard]] expr_uptr ternary_expr::clone() const {
    return std::make_unique<ternary_expr>(m_condition->clone(), m_then->clone(), m_else->clone());
}

VAR_OPTIMIZATION_ACTION ternary_expr::var_optimization_pass(var_optimization_env& env) noexcept {
    expression::check_var_optimization(&m_condition, env);
    expression::check_var_optimization(&m_then, env);
    expression::check_var_optimization(&m_else, env);
    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION ternary_expr::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    expression::check_foreach_optimization(&m_condition, env);
    expression::check_foreach_optimization(&m_then, env);
    expression::check_foreach_optimization(&m_else, env);
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

}