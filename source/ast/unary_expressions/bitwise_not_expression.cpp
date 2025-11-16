#include "ast/unary_expressions/bitwise_not_expression.h"

namespace dconstruct::ast {
void bitwise_not_expr::pseudo_c(std::ostream &os) const {
    os << '~' << m_rhs;
}

void bitwise_not_expr::pseudo_py(std::ostream &os) const {
    os << '~' << m_rhs;
}

[[nodiscard]] expr_uptr bitwise_not_expr::simplify() const {
    return nullptr;
}
}