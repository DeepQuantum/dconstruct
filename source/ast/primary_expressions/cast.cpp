#include "ast/primary_expressions/cast.h"


namespace dconstruct::ast {

void cast_expr::pseudo(std::ostream& os) const {
    os << '(' << type_to_declaration_string(m_type.value()) << ')' << *m_expr; 
}

void cast_expr::ast(std::ostream& os) const {
    os << "cast[type=" << type_to_declaration_string(m_type.value()) << ",expr=" << *m_expr << ']';
}

[[nodiscard]] expr_uptr cast_expr::simplify() const {
    return std::make_unique<cast_expr>(m_type.value(), m_expr->simplify());
}


[[nodiscard]] expr_uptr cast_expr::clone() const {
    return std::make_unique<cast_expr>(m_type.value(), m_expr->clone());
}

[[nodiscard]] b8 cast_expr::equals(const expression &rhs) const noexcept {
    const cast_expr* rhs_ptr = dynamic_cast<const cast_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_type == rhs_ptr->m_type && m_expr == rhs_ptr->m_expr;
}

[[nodiscard]] std::optional<full_type> cast_expr::compute_type(const compiler::environment& env) const {
    return m_type;
};


}