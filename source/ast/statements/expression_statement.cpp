#include "ast/statements/expression_statement.h"

namespace dconstruct::ast {

void expression_stmt::pseudo_c(std::ostream& os) const {
    os << *m_expression << ';';
}

void expression_stmt::pseudo_py(std::ostream& os) const {
    os << *m_expression << '\n';
}

void expression_stmt::pseudo_racket(std::ostream& os) const {
    os << *m_expression;
}

[[nodiscard]] bool expression_stmt::equals(const statement& rhs) const noexcept {
    const expression_stmt* rhs_ptr = dynamic_cast<const expression_stmt*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return *m_expression == *rhs_ptr->m_expression;
}

[[nodisarc]] std::unique_ptr<statement> expression_stmt::clone() const noexcept {
    return std::make_unique<expression_stmt>(m_expression->clone()); 
}

VAR_OPTIMIZATION_ACTION expression_stmt::var_optimization_pass(var_optimization_env& env) noexcept {
    expression::check_var_optimization(&m_expression, env);
    return VAR_OPTIMIZATION_ACTION::NONE; 
}

FOREACH_OPTIMIZATION_ACTION expression_stmt::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    return m_expression->foreach_optimization_pass(env);
}

MATCH_OPTIMIZATION_ACTION expression_stmt::match_optimization_pass(match_optimization_env& env) noexcept {
    return m_expression->match_optimization_pass(env);
}


}