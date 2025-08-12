#include "ast/binary_expressions/assign.h"

namespace dconstruct::ast {    
[[nodiscard]] std::unique_ptr<expression> assign_expr::simplify() const {
    return m_rhs->simplify();
}

[[nodiscard]] std::optional<ast::full_type> assign_expr::compute_type(const compiler::environment& env) const {
    return m_rhs->compute_type(env);
}

}