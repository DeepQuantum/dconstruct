#include "ast/cast.h"


namespace dconstruct::ast {

void cast_expr::pseudo_c(std::ostream& os) const {
    os << '(' << type_to_declaration_string(m_castType) << ')' << *m_rhs; 
}

void cast_expr::pseudo_py(std::ostream& os) const {
    os << type_to_declaration_string(m_castType) << '(' << *m_rhs << ')';
}

void cast_expr::pseudo_racket(std::ostream& os) const {
	os << type_to_declaration_string(m_castType) << "-> " << *m_rhs;  
}

[[nodiscard]] expr_uptr cast_expr::simplify() const {
    return std::make_unique<cast_expr>(m_castType, m_rhs->simplify());
}

[[nodiscard]] expr_uptr cast_expr::clone() const noexcept {
    return std::make_unique<cast_expr>(m_castType, m_rhs->clone());
}

[[nodiscard]] full_type cast_expr::compute_type_unchecked(const type_environment& env) const noexcept {
    return m_castType;
};

[[nodiscard]] semantic_check_res cast_expr::compute_type_checked(type_environment& env) const noexcept {
    semantic_check_res expr_type = m_rhs->get_type_checked(env);
    if (!expr_type) {
        return expr_type;
    }

    if (m_castType == *expr_type) {
        return m_castType;
    }

    const std::optional<std::string> invalid_cast = std::visit([](auto&& cast_type, auto&& expr_type) -> std::optional<std::string> {
        using cast_t = std::decay_t<decltype(cast_type)>;
        using expr_t = std::decay_t<decltype(expr_type)>;

        if constexpr (is_pointer<cast_t> && is_pointer<expr_t>) {
            return std::nullopt;
        } else if constexpr (is_primitive<cast_t> && is_primitive<expr_t>) {
            if (is_arithmethic<cast_t> && is_arithmetic<expr_t>) {
                return std::nullopt;
            }
            return "cannot cast between primitive types " + type_to_declaration_string(lhs_type) + " and " + type_to_declaration_string(rhs_type);
        } else {
            return "cannot cast expression type " + type_to_declaration_string(expr_type) + " to " + type_to_declaration_string(cast_type);
        }

    }, m_castType, *expr_type);

    if (!invalid_cast) {
        return m_castType;
    }
    return std::unexpected{semantic_check_error{*invalid_cast, this}};
}

[[nodiscard]] bool cast_expr::equals(const expression& other) const noexcept {
    return false;
}

[[nodiscard]] u16 cast_expr::calc_complexity() const noexcept {
    return 1 + m_rhs->get_complexity();
}

[[nodiscard]] expr_uptr expression::new_cast(const ast::full_type& type, const expression& expr) const noexcept { 
    return std::make_unique<cast_expr>(type, expr.get_grouped());
}

[[nodiscard]] expr_uptr cast_expr::new_cast(const ast::full_type& type, const expression&) const noexcept {
    return std::make_unique<cast_expr>(type, m_rhs->clone());
}

VAR_OPTIMIZATION_ACTION cast_expr::var_optimization_pass(var_optimization_env& env) noexcept {
    env.check_action(&m_rhs);
    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION cast_expr::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    env.check_action(&m_rhs);
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

}