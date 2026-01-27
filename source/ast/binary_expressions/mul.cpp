#include "ast/binary_expressions/mul.h"

namespace dconstruct::ast {

[[nodiscard]] expr_uptr mul_expr::simplify() const{ 
    return nullptr;
}

[[nodiscard]] semantic_check_res mul_expr::compute_type_checked(compiler::scope& env) const noexcept {
    const semantic_check_res lhs_type = m_lhs->get_type_checked(env);

    if (!lhs_type) {
        return lhs_type;
    }

    const semantic_check_res rhs_type = m_rhs->get_type_checked(env);

    if (!rhs_type) {
        return rhs_type;
    }

    const std::expected<full_type, std::string> valid_mul = is_valid_binary_op(*lhs_type, *rhs_type, "*");

    if (!valid_mul) {
        return std::unexpected{semantic_check_error{valid_mul.error(), this}};
    }

    return *valid_mul;
}

[[nodiscard]] emission_res mul_expr::emit_dc(compiler::function& fn, compiler::global_state& global, const std::optional<reg_idx> destination, const std::optional<u8> arg_pos) const noexcept {
    const emission_res lhs = m_lhs->emit_dc(fn, global);
    if (!lhs) {
        return lhs;
    }

    const emission_res rhs = m_rhs->emit_dc(fn, global);
    if (!rhs) {
        return rhs;
    }

    assert(std::holds_alternative<primitive_type>(*m_type));
    const Opcode mul_opcode = is_integral(std::get<primitive_type>(*m_type).m_type) ? Opcode::IMul : Opcode::FMul;
    
    reg_idx mul_destination;
    if (destination) {
        mul_destination = *destination;
    } else {
        const emission_res new_destination = fn.get_next_unused_register();
        if (!new_destination) {
            return new_destination;
        }
        mul_destination = *new_destination;
    }

    fn.emit_instruction(mul_opcode, mul_destination, *lhs, *rhs);
    fn.free_register(*lhs);
    fn.free_register(*rhs);

    return mul_destination;
}

}