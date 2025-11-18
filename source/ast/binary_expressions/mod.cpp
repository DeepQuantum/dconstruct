#include "ast/binary_expressions/mod.h"


namespace dconstruct::ast {

[[nodiscard]] expr_uptr mod_expr::simplify() const { 
    return nullptr;
}


}
