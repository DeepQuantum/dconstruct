#include "ast/binary_expressions/mul.h"

namespace dconstruct::ast {

[[nodiscard]] expr_uptr mul_expr::simplify() const{ 
    return nullptr;
}

}