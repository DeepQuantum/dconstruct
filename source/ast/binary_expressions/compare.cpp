#include "ast/binary_expressions/compare.h"

namespace dconstruct::ast {

[[nodiscard]] expr_uptr compare_expr::simplify() const {
    return nullptr;
}

[[nodiscard]] full_type compare_expr::compute_type_unchecked(const compilation::scope&) const noexcept {
    return make_type_from_prim(primitive_kind::BOOL);
}


[[nodiscard]] semantic_check_res compare_expr::compute_type_checked(compilation::scope& env) const noexcept {
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
        
        if constexpr (!is_primitive<lhs_t>) {
            return "cannot compare non-primitive left hand side type " + type_to_declaration_string(lhs_type);
        } else if constexpr (!is_primitive<rhs_t>) {
            return "cannot compare non-primitive right hand side type " + type_to_declaration_string(lhs_type);
        } else if (is_integral(lhs_type.m_type) && is_integral(rhs_type.m_type)) {
            return std::nullopt;
        } else if (is_floating_point(lhs_type.m_type) && is_floating_point(rhs_type.m_type)) {
            return std::nullopt;
        } else { 
            return "cannot compare left hand side type " + type_to_declaration_string(lhs_type) + " with right and side type " + type_to_declaration_string(rhs_type);
        }
    }, *lhs_type, *rhs_type);

    if (!invalid_compare) {
        return make_type_from_prim(primitive_kind::BOOL);
    }
    
    return std::unexpected{semantic_check_error{*invalid_compare, this}};
}

[[nodiscard]] emission_res compare_expr::emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination) const noexcept {
    const emission_res lhs = m_lhs->emit_dc(fn, global);
    if (!lhs) {
        return lhs;
    }

    const emission_res rhs = m_rhs->emit_dc(fn, global);
    if (!rhs) {
        return rhs;
    }
    
    const emission_res comp_destination = destination ? *destination : fn.get_next_unused_register();
    if (!comp_destination) {
        return comp_destination;
    }

    const bool integral = is_integral(std::get<primitive_type>(*m_type).m_type);

    Opcode opcode;
    if (m_operator.m_lexeme == ">") {
        opcode = integral ? Opcode::IGreaterThan : Opcode::FGreaterThan;
    } else if (m_operator.m_lexeme == "<") {
        opcode = integral ? Opcode::ILessThan : Opcode::FLessThan;
    } else if (m_operator.m_lexeme == ">=") {
        opcode = integral ? Opcode::IGreaterThanEqual : Opcode::FGreaterThanEqual;
    } else if (m_operator.m_lexeme == "<=") {
        opcode = integral ? Opcode::ILessThanEqual : Opcode::FLessThanEqual;
    }

    fn.emit_instruction(opcode, *comp_destination, *lhs, *rhs);
    
    fn.free_register(*lhs);
    fn.free_register(*rhs);

    return comp_destination;
}

}