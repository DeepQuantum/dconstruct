#include "ast/binary_expressions/cast.h"


namespace dconstruct::ast {

void cast_expr::pseudo(std::ostream& os) const {
    os << '(' << type_to_declaration_string(m_type) << ')' << *m_rhs; 
}

void cast_expr::ast(std::ostream& os) const {
    os << "cast[type=" << type_to_declaration_string(m_type) << ",expr=" << *m_rhs << ']';
}

[[nodiscard]] expr_uptr cast_expr::simplify() const {
    return std::make_unique<cast_expr>(m_type, m_rhs->simplify());
}

[[nodiscard]] expr_uptr cast_expr::clone() const noexcept {
    return std::make_unique<cast_expr>(m_type, m_rhs->clone());
}

[[nodiscard]] full_type cast_expr::compute_type(const compiler::environment& env) const {
    return m_type;
};

[[nodiscard]] b8 cast_expr::equals(const expression& other) const noexcept {
    return false;
}

[[nodiscard]] u16 cast_expr::complexity() const noexcept {
    return 1 + m_rhs->complexity();
}



}