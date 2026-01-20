#include "ast/primary_expressions/subscript.h"
#include "ast/optimization/foreach_optimization.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"

namespace dconstruct::ast {

void subscript_expr::pseudo_c(std::ostream& os) const {
    os << *m_lhs << "[" << *m_rhs << "]";
}

void subscript_expr::pseudo_py(std::ostream& os) const {
    os << *m_lhs << "[" << *m_rhs << "]";
}

void subscript_expr::pseudo_racket(std::ostream& os) const {
    os << *m_lhs << "[" << *m_rhs << "]";
}

[[nodiscard]] expr_uptr subscript_expr::simplify() const {
    return std::make_unique<subscript_expr>(m_lhs->clone(), m_rhs->simplify());
}

[[nodiscard]] expr_uptr subscript_expr::clone() const noexcept {
    return std::make_unique<subscript_expr>(m_lhs->clone(), m_rhs->clone());
}


[[nodiscard]] bool subscript_expr::equals(const expression& rhs) const noexcept {
    const subscript_expr* rhs_ptr = dynamic_cast<const subscript_expr*>(&rhs);
    if (!rhs_ptr) {
        return false;
    }
    return m_lhs == rhs_ptr->m_lhs && m_rhs == rhs_ptr->m_rhs;
}

[[nodiscard]] u16 subscript_expr::calc_complexity() const noexcept {
    return 1 + m_lhs->get_complexity() + m_rhs->get_complexity();
}

[[nodiscard]] full_type subscript_expr::compute_type_unchecked(const compiler::scope& env) const noexcept {
    const std::optional<full_type> lhs_type = m_lhs->compute_type_unchecked(env);
    if (!lhs_type) {
        return std::monostate();
    }
    if (std::holds_alternative<ptr_type>(*lhs_type)) {
        return *std::get<ptr_type>(*lhs_type).m_pointedAt;
    } else {
        return std::monostate();
    }
}

[[nodiscard]] semantic_check_res subscript_expr::compute_type_checked(compiler::scope& scope) const noexcept {
    const std::optional<full_type> lhs_type = m_lhs->compute_type_unchecked(scope);
    return {};
}

VAR_OPTIMIZATION_ACTION subscript_expr::var_optimization_pass(var_optimization_env& env) noexcept {
    env.check_action(&m_lhs);
    env.check_action(&m_rhs);
    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION subscript_expr::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    env.check_action(&m_lhs);
    env.check_action(&m_rhs);
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

}