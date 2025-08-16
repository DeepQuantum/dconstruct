#include "ast/binary_expressions/div.h"


namespace dconstruct::ast {

[[nodiscard]] expr_uptr div_expr::simplify() const { 
    return nullptr;
}


}
