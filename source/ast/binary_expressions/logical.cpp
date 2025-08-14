#include "ast/binary_expressions/logical.h"


namespace dconstruct::ast {

[[nodiscard]] std::unique_ptr<ast::expression> logical_expr::simplify() const {
    return nullptr;
}


}