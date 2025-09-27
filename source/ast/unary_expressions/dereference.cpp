#include "ast/unary_expressions/dereference_expression.h"


namespace dconstruct::ast {

    void dereference_expr::pseudo(std::ostream& os) const {
        os << '*' << *m_rhs;
    }

    void dereference_expr::ast(std::ostream& os) const {
        os << "dereference[" << *m_rhs << ']';
    }

    [[nodiscard]] expr_uptr dereference_expr::simplify() const {
        return std::make_unique<ast::dereference_expr>(m_operator, m_rhs->simplify());
    }
}