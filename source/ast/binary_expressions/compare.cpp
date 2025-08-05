#include "ast/binary_expressions/compare.h"

namespace dconstruct::ast {

[[nodiscard]] std::unique_ptr<expression> compare_expr::eval() const {
    return nullptr;
}
}