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

[[nodiscard]] u16 cast_expr::complexity() const noexcept {
    return 1 + m_rhs->complexity();
}

VAR_FOLDING_ACTION cast_expr::decomp_optimization_pass(second_pass_env& env) noexcept {
    expression::check_optimization(&m_rhs, env);
    return VAR_FOLDING_ACTION::NONE;
}



}