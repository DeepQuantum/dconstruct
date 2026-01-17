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

[[nodiscard]] full_type ternary_expr::compute_type_unchecked(const type_environment& env) const noexcept {
    return make_type_from_prim(primitive_kind::BOOL);
}

[[nodiscard]] semantic_check_res ternary_expr::compute_type_checked(type_environment& env) const noexcept {
    const semantic_check_res condition_type = m_condition->get_type_checked(env);
    
    if (!condition_type) {
        return condition_type;
    }

    const std::optional<std::string> invalid_condition = std::visit([](auto&& cond) -> std::optional<std::string> {
        using cond_t = std::decay_t<decltype(cond)>;

        if constexpr (is_primitive<cond_t>) {
            if constexpr (is_arithmethic(cond)) {
                return std::nullopt;
            }
            return "ternary condition must be of arithmetic type, but got " + type_to_declaration_string(cond);
        }
        return "ternary condition must be of arithmetic type, but got " + type_to_declaration_string(cond);

    }, *condition_type);

    if (invalid_condition) {
        return std::unexpected{semantic_check_error{*invalid_condition, m_condition.get()}};
    }

    const semantic_check_res then_type = m_then->get_type_checked(env);

    if (!then_type) {
        return then_type;
    }

    const semantic_check_res else_type = m_else->get_type_checked(env);

    if (!else_type) {
        return else_type;
    }

    if (*then_type != *else_type) {
        return std::unexpected{semantic_check_error{"else type of ternary expression " + type_to_declaration_string(*else_type) + " must equal then type " + type_to_declaration_string(*then_type), m_else.get()}};
    }

    return *then_type;
}

[[nodiscard]] expr_uptr ternary_expr::clone() const {
    return std::make_unique<ternary_expr>(m_condition->clone(), m_then->clone(), m_else->clone());
}

VAR_OPTIMIZATION_ACTION ternary_expr::var_optimization_pass(var_optimization_env& env) noexcept {
    env.check_action(&m_condition);
    env.check_action(&m_then);
    env.check_action(&m_else);

    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION ternary_expr::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    env.check_action(&m_condition);
    env.check_action(&m_then);
    env.check_action(&m_else);
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

}