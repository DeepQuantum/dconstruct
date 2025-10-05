#include "ast/binary_expressions/cast.h"


namespace dconstruct::ast {

void cast_expr::pseudo(std::ostream& os) const {
    os << '(' << *m_lhs << ')' << *m_rhs; 
}

void cast_expr::ast(std::ostream& os) const {
    os << "cast[type=" << *m_lhs << ",expr=" << *m_rhs << ']';
}

[[nodiscard]] expr_uptr cast_expr::simplify() const {
    return std::make_unique<cast_expr>(m_operator, m_lhs->clone(), m_rhs->simplify());
}

[[nodiscard]] full_type cast_expr::compute_type(const compiler::environment& env) const {
    return m_lhs->get_type(env);
};


}