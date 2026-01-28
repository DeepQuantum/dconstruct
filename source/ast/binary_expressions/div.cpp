#include "ast/binary_expressions/div.h"


namespace dconstruct::ast {

[[nodiscard]] expr_uptr div_expr::simplify() const { 
    return nullptr;
}

[[nodiscard]] semantic_check_res div_expr::compute_type_checked(compilation::scope& env) const noexcept {
    const semantic_check_res lhs_type = m_lhs->get_type_checked(env);

    if (!lhs_type) {
        return lhs_type;
    }

    const semantic_check_res rhs_type = m_rhs->get_type_checked(env);

    if (!rhs_type) {
        return rhs_type;
    }

    const std::expected<full_type, std::string> valid_div = std::visit([](auto&& lhs_type, auto&& rhs_type) -> std::expected<full_type, std::string> {
        using lhs_t = std::decay_t<decltype(lhs_type)>;
        using rhs_t = std::decay_t<decltype(rhs_type)>;
        
        if constexpr (!is_primitive<lhs_t>) {
            return std::unexpected{"cannot divide non-primitive type " + type_to_declaration_string(lhs_type)};
        } else if constexpr (!is_primitive<rhs_t>) {
            return std::unexpected{"cannot divide non-primitive type " + type_to_declaration_string(rhs_type)};
        } else if (is_integral(lhs_type.m_type)) {
            if (is_integral(rhs_type.m_type)) {
                return make_type_from_prim(primitive_kind::U64);
            }
            return std::unexpected{"cannot divide integral type " + type_to_declaration_string(lhs_type) + " with non-integral type " + type_to_declaration_string(rhs_type)};
        } else if (is_floating_point(lhs_type.m_type)) {
            if (is_floating_point(rhs_type.m_type)) {
                return make_type_from_prim(primitive_kind::F32);
            }
            return std::unexpected{"cannot divide floating point type " + type_to_declaration_string(lhs_type) + " with non-floating point type " + type_to_declaration_string(rhs_type)};
        } else {
            return std::unexpected{"cannot divide " + type_to_declaration_string(lhs_type)};
        }
    }, *lhs_type, *rhs_type);

    if (!valid_div) {
        return std::unexpected{semantic_check_error{valid_div.error(), this}};
    }

    return *valid_div;
}

[[nodiscard]] emission_res div_expr::emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination) const noexcept {
    const emission_res lhs = m_lhs->emit_dc(fn, global);
    if (!lhs) {
        return lhs;
    }

    const emission_res rhs = m_rhs->emit_dc(fn, global);
    if (!rhs) {
        return rhs;
    }

    assert(std::holds_alternative<primitive_type>(*m_type));
    const Opcode div_opcode = is_integral(std::get<primitive_type>(*m_type).m_type) ? Opcode::IDiv : Opcode::FDiv;
    
    const emission_res div_destination = fn.get_destination(destination);
    if (!div_destination) {
        return div_destination;
    }

    fn.emit_instruction(div_opcode, *div_destination, *lhs, *rhs);
    fn.free_register(*lhs);
    fn.free_register(*rhs);

    return *div_destination;
}

}
