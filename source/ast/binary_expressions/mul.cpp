#include "ast/binary_expressions/mul.h"

namespace dconstruct::ast {

[[nodiscard]] std::unique_ptr<expression> mul_expr::simplify() const{ 
    return nullptr;
}

}