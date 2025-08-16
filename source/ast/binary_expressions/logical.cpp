#include "ast/binary_expressions/logical.h"


namespace dconstruct::ast {

[[nodiscard]] expr_uptr logical_expr::simplify() const {
    return nullptr;
}


}