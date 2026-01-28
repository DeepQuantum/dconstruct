#include "ast/binary_expressions/mod.h"


namespace dconstruct::ast {

[[nodiscard]] expr_uptr mod_expr::simplify() const { 
    return nullptr;
}

[[nodiscard]] semantic_check_res mod_expr::compute_type_checked(compilation::scope& env) const noexcept {
    const semantic_check_res lhs_type = m_lhs->get_type_checked(env);

    if (!lhs_type) {
        return lhs_type;
    }

    const semantic_check_res rhs_type = m_rhs->get_type_checked(env);

    if (!rhs_type) {
        return rhs_type;
    }

    const std::expected<full_type, std::string> valid_mod = std::visit([](auto&& lhs_type, auto&& rhs_type) -> std::expected<full_type, std::string> {
        using lhs_t = std::decay_t<decltype(lhs_type)>;
        using rhs_t = std::decay_t<decltype(rhs_type)>;
        
        if constexpr (!is_primitive<lhs_t>) {
            return std::unexpected{"cannot mod non-primitive type " + type_to_declaration_string(lhs_type)};
        } else if constexpr (!is_primitive<rhs_t>) {
            return std::unexpected{"cannot mod non-primitive type " + type_to_declaration_string(rhs_type)};
        } else if (is_integral(lhs_type.m_type)) {
            if (is_integral(rhs_type.m_type)) {
                return make_type_from_prim(primitive_kind::U64);
            }
            return std::unexpected{"cannot mod integral type " + type_to_declaration_string(lhs_type) + " with non-integral type " + type_to_declaration_string(rhs_type)};
        } else if (is_floating_point(lhs_type.m_type)) {
            if (is_floating_point(rhs_type.m_type)) {
                return make_type_from_prim(primitive_kind::F32);
            }
            return std::unexpected{"cannot mod floating point type " + type_to_declaration_string(lhs_type) + " with non-floating point type " + type_to_declaration_string(rhs_type)};
        } else {
            return std::unexpected{"cannot mod " + type_to_declaration_string(lhs_type)};
        }
    }, *lhs_type, *rhs_type);

    if (!valid_mod) {
        return std::unexpected{semantic_check_error{valid_mod.error(), this}};
    }

    return *valid_mod;
}

[[nodiscard]] emission_res mod_expr::emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination) const noexcept {
    const emission_res lhs = m_lhs->emit_dc(fn, global);
    if (!lhs) {
        return lhs;
    }

    const emission_res rhs = m_rhs->emit_dc(fn, global);
    if (!rhs) {
        return rhs;
    }

    assert(std::holds_alternative<primitive_type>(*m_type));
    const Opcode mod_opcode = is_integral(std::get<primitive_type>(*m_type).m_type) ? Opcode::IMod : Opcode::FMod;

    const emission_res mod_destination = fn.get_destination(destination);
    if (!mod_destination) {
        return mod_destination;
    }

    fn.emit_instruction(mod_opcode, *mod_destination, *lhs, *rhs);
    fn.free_register(*lhs);
    fn.free_register(*rhs);

    return *mod_destination;
}


}
