#include "ast/primary_expressions/dereference.h"


namespace dconstruct::ast {

    void dereference_expr::pseudo(std::ostream& os) const {
        os << '*' << *m_expr;
    }

    void dereference_expr::ast(std::ostream& os) const {
        os << "dereference[" << *m_expr << ']';
    }

    [[nodiscard]] expr_uptr dereference_expr::simplify() const {
        return std::make_unique<ast::dereference_expr>(m_expr->simplify());
    }


    [[nodiscard]] expr_uptr dereference_expr::clone() const {
        return std::make_unique<ast::dereference_expr>(m_expr->clone());
    }


    [[nodiscard]] b8 dereference_expr::equals(const expression& rhs) const noexcept {
        const dereference_expr* rhs_ptr = dynamic_cast<const dereference_expr*>(&rhs);
        if (rhs_ptr == nullptr) {
            return false;
        }
        return m_expr == rhs_ptr->m_expr;
    }

    [[nodiscard]] std::optional<full_type> dereference_expr::compute_type(const compiler::environment& env) const {
        return std::nullopt;
    };
}