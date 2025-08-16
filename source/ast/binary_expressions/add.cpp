#include "ast/binary_expressions/add.h"
#include "ast/primary_expressions/literal.h"

namespace dconstruct::ast {

[[nodiscard]] expr_uptr add_expr::simplify() const {
    expr_uptr lhs_ptr = m_lhs->simplify();
    expr_uptr rhs_ptr = m_rhs->simplify();

    if (const literal* lhs_num_lit = dynamic_cast<const literal*>(lhs_ptr.get())) {
        if (const literal* rhs_num_lit = dynamic_cast<const literal*>(rhs_ptr.get())) {
            // int a = 0;
            // sid b = #ellie;
            if (*lhs_num_lit != *rhs_num_lit) {
                return nullptr;
            }
            //if (lhs_num_lit)
        }
    }
    return std::make_unique<add_expr>(m_operator, std::move(lhs_ptr), std::move(rhs_ptr));
}

}