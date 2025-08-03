#include "add.h"
#include "literal.h"

namespace dconstruct::ast {

[[nodiscard]] b8 add_expr::equals(const expression& rhs) const noexcept {
    const add_expr* rhs_ptr = dynamic_cast<const add_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_lhs == rhs_ptr->m_lhs && m_rhs == rhs_ptr->m_rhs;
}

[[nodiscard]] std::unique_ptr<expression> add_expr::eval() const {
    std::unique_ptr<expression> lhs_ptr = m_lhs->eval();
    std::unique_ptr<expression> rhs_ptr = m_rhs->eval();

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
    return std::make_unique<add_expr>(std::move(lhs_ptr), std::move(rhs_ptr));
}

}