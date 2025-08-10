#include "ast/binary_expressions/div.h"


namespace dconstruct::ast {

[[nodiscard]] std::unique_ptr<expression> div_expr::simplify() const { 
    return nullptr;
}

}
