#include "ast/binary_expressions/bitwise_xor.h"


namespace dconstruct::ast {

[[nodiscard]] semantic_check_res bitwise_xor_expr::compute_type_checked(type_environment& env) const noexcept {
    const semantic_check_res lhs_type = m_lhs->get_type_checked(env);

    if (!lhs_type) {
        return lhs_type;
    }

    const semantic_check_res rhs_type = m_rhs->get_type_checked(env);

    if (!rhs_type) {
        return rhs_type;
    }

    const std::optional<std::string> invalid_bitwise_xor = std::visit([](auto&& lhs_type, auto&& rhs_type) -> std::optional<std::string> {
        using lhs_t = std::decay_t<decltype(lhs_type)>;
        using rhs_t = std::decay_t<decltype(rhs_type)>;
        
        if constexpr (!is_primitive<lhs_t>) {
            return "cannot bitwise-xor non-primitive type " + type_to_declaration_string(lhs_type);
        } else if constexpr (!is_primitive<rhs_t>) {
            return "cannot bitwise-xor non-primitive type " + type_to_declaration_string(rhs_type);
        } else if constexpr (is_integral<lhs_t>) {
            if constexpr (is_integral<rhs_t>) {
                return std::nullopt;
            }
            return "cannot bitwise-xor right hand side non-integral type " + type_to_declaration_string(rhs_type);
        } else {
            return "cannot bitwise-xor left hand side non-integral type " + type_to_declaration_string(rhs_type);
        }
    }, *lhs_type, *rhs_type);

    if (!invalid_bitwise_xor) {
        return make_type_from_prim(primitive_kind::U64);
    }
    
    return std::unexpected{semantic_check_error{*invalid_bitwise_xor, this}};
}


[[nodiscard]] expr_uptr bitwise_xor_expr::simplify() const { 
    return nullptr;
}


}
