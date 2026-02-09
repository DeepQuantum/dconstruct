#include "ast/binary_expressions/logical.h"


namespace dconstruct::ast {

[[nodiscard]] expr_uptr logical_expr::simplify() const {
    return nullptr;
}

[[nodiscard]] semantic_check_res logical_expr::compute_type_checked(compilation::scope& env) const noexcept {
    semantic_check_res lhs_type = m_lhs->get_type_checked(env);
    if (!lhs_type) {
        return lhs_type;
    }

    semantic_check_res rhs_type = m_rhs->get_type_checked(env);
    if (!rhs_type) {
        return rhs_type;
    }

    const std::optional<std::string> invalid_logical = std::visit([](auto&& lhs_type, auto&& rhs_type) -> std::optional<std::string> {
        using lhs_t = std::decay_t<decltype(lhs_type)>;
        using rhs_t = std::decay_t<decltype(rhs_type)>;
        
        if constexpr (!is_primitive<lhs_t>) {
            return "expected primitive type for logical op lhs but got " + type_to_declaration_string(lhs_type);
        } else if constexpr (!is_primitive<rhs_t>) {
            return "expected primitive type for logical op rhs but got " + type_to_declaration_string(rhs_type);
        } else if (is_integral(lhs_type.m_type) && is_integral(rhs_type.m_type)) {
            return std::nullopt;
        } else if (is_floating_point(lhs_type.m_type) && is_floating_point(rhs_type.m_type)) {
            return std::nullopt;
        } else { 
            return "expected compatible types for logical op but got " + type_to_declaration_string(lhs_type) + " and " + type_to_declaration_string(rhs_type);
        }
    }, *lhs_type, *rhs_type);

    if (!invalid_logical) {
        return make_type_from_prim(primitive_kind::BOOL);
    }
    
    return std::unexpected{semantic_check_error{*invalid_logical, this}};
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