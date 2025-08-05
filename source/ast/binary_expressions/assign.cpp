#include "ast/binary_expressions/assign.h"

namespace dconstruct::ast {    
[[nodiscard]] std::unique_ptr<expression> assign_expr::eval() const {
    return m_rhs->eval();
}
}