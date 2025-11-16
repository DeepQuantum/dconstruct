#include "ast/unary_expressions/increment_expression.h"


namespace dconstruct::ast {

void increment_expression::pseudo_c(std::ostream& os) const {
    os << *m_rhs << "++";
}

void increment_expression::pseudo_py(std::ostream& os) const {
    os << *m_rhs << "++";
}

[[nodiscard]] expr_uptr increment_expression::simplify() const {
    return std::make_unique<ast::increment_expression>(m_operator, m_rhs->simplify());
}



}