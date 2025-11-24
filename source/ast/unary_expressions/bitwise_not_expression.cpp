#include "ast/unary_expressions/bitwise_not_expression.h"

namespace dconstruct::ast {

[[nodiscard]] expr_uptr bitwise_not_expr::simplify() const {
    return nullptr;
}
}