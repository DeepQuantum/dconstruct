#include "ast/binary_expressions/compare.h"

namespace dconstruct::ast {

[[nodiscard]] expr_uptr compare_expr::simplify() const {
    return nullptr;
}

[[nodiscard]] full_type compare_expr::compute_type_unchecked(const type_environment&) const noexcept {
    return make_type_from_prim(primitive_kind::BOOL);
}


[[nodiscard]] semantic_check_res compare_expr::compute_type_checked(type_environment& env) const noexcept {
    const semantic_check_res lhs_type = m_lhs->get_type_checked(env);

    if (!lhs_type) {
        return lhs_type;
    }

    const semantic_check_res rhs_type = m_rhs->get_type_checked(env);

    if (!rhs_type) {
        return rhs_type;
    }


    if (*lhs_type == *rhs_type) {
        return make_type_from_prim(primitive_kind::BOOL);
    }

    const std::optional<std::string> invalid_compare = std::visit([](auto&& lhs_type, auto&& rhs_type) -> std::optional<std::string> {
        using lhs_t = std::decay_t<decltype(lhs_type)>;
        using rhs_t = std::decay_t<decltype(rhs_type)>;
        
        if constexpr (is_primitive<lhs_t> && is_primitive<rhs_t>) {
            if constexpr (is_integral(lhs_type) && is_integral(rhs_type)) {
                return std::nullopt;
            } else if constexpr (is_floating_point(lhs_type) && is_floating_point(rhs_type)) {
                return std::nullopt;
            }
            return "cannot compare left hand side type " + type_to_declaration_string(lhs_type) + " with right and side type " + type_to_declaration_string(rhs_type);
        } else { 
            return "cannot compare left hand side type " + type_to_declaration_string(lhs_type) + " with right and side type " + type_to_declaration_string(rhs_type);
        }
    }, *lhs_type, *rhs_type);

    if (!invalid_compare) {
        return make_type_from_prim(primitive_kind::BOOL);
    }
    
    return std::unexpected{semantic_check_error{*invalid_compare, this}};
}

}