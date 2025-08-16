#include "ast/binary_expressions/sub.h"

namespace dconstruct::ast {

[[nodiscard]] expr_uptr sub_expr::simplify() const { 
    return nullptr;
}

}