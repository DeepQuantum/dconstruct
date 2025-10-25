#include "ast/unary_expressions/increment_expression.h"


namespace dconstruct::ast {

void increment_expression::pseudo(std::ostream& os) const {
    os << *m_rhs << "++";
}

void increment_expression::ast(std::ostream& os) const {
    os << "increment[" << *m_rhs << ']';
}

[[nodiscard]] expr_uptr increment_expression::simplify() const {
    return std::make_unique<ast::increment_expression>(m_operator, m_rhs->simplify());
}



}