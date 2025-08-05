#include "ast/binary_expressions/sub.h"

namespace dconstruct::ast {

[[nodiscard]] std::unique_ptr<expression> sub_expr::eval() const { 
    return nullptr;
}

}