#include "ast/binary_expressions/shift.h"


namespace dconstruct::ast {

[[nodiscard]] expr_uptr shift_expr::simplify() const { 
    return nullptr;
}


}
