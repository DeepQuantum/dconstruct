#include "ast/binary_expressions/logical.h"


namespace dconstruct::ast {

[[nodiscard]] expr_uptr logical_expr::simplify() const {
    return nullptr;
}
//
//void logical_expr::pseudo_c(std::ostream& os) const {
//
//}
//
//void logical_expr::pseudo_py(std::ostream& os) const {
//
//}
//
//void logical_expr::pseudo_racket(std::ostream& os) const {
//
//}

}