#include "ast/binary_expressions/compare.h"

namespace dconstruct::ast {

[[nodiscard]] expr_uptr compare_expr::simplify() const {
    return nullptr;
}

[[nodiscard]] full_type compare_expr::compute_type(const type_environment&) const {
    return make_type_from_prim(primitive_kind::BOOL);
}

}