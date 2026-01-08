#include "ast/cast.h"


namespace dconstruct::ast {

void cast_expr::pseudo_c(std::ostream& os) const {
    os << '(' << type_to_declaration_string(m_type) << ')' << *m_rhs; 
}

void cast_expr::pseudo_py(std::ostream& os) const {
    os << type_to_declaration_string(m_type) << '(' << *m_rhs << ')';
}

void cast_expr::pseudo_racket(std::ostream& os) const {
	os << type_to_declaration_string(m_type) << "-> " << *m_rhs;  
}

[[nodiscard]] expr_uptr cast_expr::simplify() const {
    return std::make_unique<cast_expr>(m_type, m_rhs->simplify());
}

[[nodiscard]] expr_uptr cast_expr::clone() const noexcept {
    return std::make_unique<cast_expr>(m_type, m_rhs->clone());
}

[[nodiscard]] full_type cast_expr::compute_type(const type_environment& env) const {
    return m_type;
};

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