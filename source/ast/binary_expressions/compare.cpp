#include "ast/binary_expressions/compare.h"

namespace dconstruct::ast {

[[nodiscard]] expr_uptr compare_expr::simplify() const {
    return nullptr;
}

}