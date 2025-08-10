#include "ast/binary_expressions/assign.h"

namespace dconstruct::ast {    
[[nodiscard]] std::unique_ptr<expression> assign_expr::simplify() const {
    return m_rhs->simplify();
}
}