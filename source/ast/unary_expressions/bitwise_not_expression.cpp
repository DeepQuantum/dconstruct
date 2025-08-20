#include "ast/unary_expressions/bitwise_not_expression.h"

namespace dconstruct::ast {
void bitwise_not_expr::pseudo(std::ostream &os) const {
    os << '~' << m_rhs;
}

void bitwise_not_expr::ast(std::ostream &os) const {
    os << "bitwise_not[" << m_rhs << ']'; 
}

[[nodiscard]] expr_uptr bitwise_not_expr::simplify() const {
    return nullptr;
}
}