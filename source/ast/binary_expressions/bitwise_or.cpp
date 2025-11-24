#include "ast/binary_expressions/bitwise_or.h"


namespace dconstruct::ast {

[[nodiscard]] expr_uptr bitwise_or_expr::simplify() const { 
    return nullptr;
}


}
