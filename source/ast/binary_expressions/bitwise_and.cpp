#include "ast/binary_expressions/bitwise_and.h"


namespace dconstruct::ast {

[[nodiscard]] expr_uptr bitwise_and_expr::simplify() const { 
    return nullptr;
}


}
