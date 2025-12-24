#include "ast/statements/return.h"

namespace dconstruct::ast {

void return_stmt::pseudo_c(std::ostream& os) const {
    os << "return " << *m_expr << ';';
}

void return_stmt::pseudo_py(std::ostream& os) const {
    os << "return " << *m_expr << '\n';
}

void return_stmt::pseudo_racket(std::ostream& os) const {
    return;
}

[[nodiscard]] bool return_stmt::equals(const statement& rhs) const noexcept {
    const return_stmt* rhs_ptr = dynamic_cast<const return_stmt*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_expr == rhs_ptr->m_expr;
}

[[nodiscard]] std::unique_ptr<statement> return_stmt::clone() const noexcept {
    return std::make_unique<return_stmt>(m_expr->clone());
}

OPTIMIZATION_ACTION return_stmt::decomp_optimization_pass(optimization_pass_context& optimization_ctx) noexcept {
    expression::check_optimization(&m_expr, optimization_ctx);
    return OPTIMIZATION_ACTION::NONE;
}

}