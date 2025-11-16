#include "ast/unary_expressions/dereference_expression.h"


namespace dconstruct::ast {

    void dereference_expr::pseudo_c(std::ostream& os) const {
        os << '*' << *m_rhs;
    }

    void dereference_expr::pseudo_py(std::ostream& os) const {
        os << '*' << *m_rhs;
    }

    [[nodiscard]] expr_uptr dereference_expr::simplify() const {
        return std::make_unique<ast::dereference_expr>(m_operator, m_rhs->simplify());
    }

    [[nodiscard]] full_type dereference_expr::compute_type(const compiler::environment& env) const {
        const auto rhs_type = m_rhs->compute_type(env);
        if (std::holds_alternative<ast::ptr_type>(rhs_type)) {
            return *std::get<ast::ptr_type>(rhs_type).m_pointedAt;
        }
        return ast::full_type{std::monostate()};
    }
}