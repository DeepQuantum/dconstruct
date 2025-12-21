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

bool return_stmt::decomp_optimization_pass(second_pass_env& env) noexcept {
    if (m_expr->decomp_optimization_pass(env)) {
        env->lookup(static_cast<identifier&>(*m_expr).m_name.m_lexeme)->m_firstUsageSite = &m_expr;
    }
    return false;
}

}