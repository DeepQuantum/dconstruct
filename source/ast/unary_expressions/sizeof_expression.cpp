#include "ast/primary_expressions/sizeof_expression.h"

namespace dconstruct::ast {

[[nodiscard]] bool sizeof_expr::equals(const expression& rhs) const noexcept {
    const sizeof_expr* rhs_ptr = dynamic_cast<const sizeof_expr*>(&rhs);
    if (!rhs_ptr) {
        return false;
    }
    return m_operand == rhs_ptr->m_operand;
}

[[nodiscard]] u16 sizeof_expr::calc_complexity() const noexcept {
    if (std::holds_alternative<expr_uptr>(m_operand)) {
        return 1 + std::get<expr_uptr>(m_operand)->get_complexity();
    }
    return 1;
}

[[nodiscard]] expr_uptr sizeof_expr::clone() const noexcept {
    if (std::holds_alternative<full_type>(m_operand)) {
        return std::make_unique<sizeof_expr>(std::get<full_type>(m_operand));
    } else {
        return std::make_unique<sizeof_expr>(std::get<expr_uptr>(m_operand)->clone());
    }
}

[[nodiscard]] expr_uptr sizeof_expr::simplify() const {
    return clone();
}

void sizeof_expr::pseudo_c(std::ostream& os) const {
    os << "sizeof(";
    if (std::holds_alternative<full_type>(m_operand)) {
        os << type_to_declaration_string(std::get<full_type>(m_operand));
    } else {
        os << *std::get<expr_uptr>(m_operand);
    }
    os << ")";
}

void sizeof_expr::pseudo_py(std::ostream& os) const {
    if (std::holds_alternative<full_type>(m_operand)) {
        os << "sizeof(" << type_to_declaration_string(std::get<full_type>(m_operand)) << ")";
    } else {
        os << "sys.getsizeof(" << *std::get<expr_uptr>(m_operand) << ")";
    }
}

void sizeof_expr::pseudo_racket(std::ostream& os) const {
    os << "(sizeof ";
    if (std::holds_alternative<full_type>(m_operand)) {
        os << type_to_declaration_string(std::get<full_type>(m_operand));
    } else {
        os << *std::get<expr_uptr>(m_operand);
    }
    os << ")";
}

[[nodiscard]] inline full_type sizeof_expr::compute_type_unchecked(const compilation::scope& env) const noexcept {
    return make_type_from_prim(primitive_kind::U64);
}

[[nodiscard]] semantic_check_res sizeof_expr::compute_type_checked(compilation::scope& env) const noexcept {
    if (std::holds_alternative<expr_uptr>(m_operand)) {
        const semantic_check_res op_res = std::get<expr_uptr>(m_operand)->compute_type_checked(env);
        if (!op_res) {
            return op_res;
        }
    }
    return make_type_from_prim(primitive_kind::U64);
}


[[nodiscard]] emission_res sizeof_expr::emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination) const noexcept {
    u64 type_size;

    if (std::holds_alternative<full_type>(m_operand)) {
        type_size = get_size(std::get<full_type>(m_operand));
    } else {
        std::optional<full_type> op_type = std::get<expr_uptr>(m_operand)->get_type();
        assert(op_type);
        type_size = get_size(*op_type);
    }

    if (type_size > std::numeric_limits<u16>::max()) {
        return std::unexpected{"expected type with size less than " + std::to_string(std::numeric_limits<u16>::max()) + " but got size " + std::to_string(type_size)};
    }

    const emission_res sizeof_dest = fn.get_destination(destination);
    if (!sizeof_dest) {
        return sizeof_dest;
    }

    const u8 lo = static_cast<u16>(type_size) & 0xFF;
    const u8 hi = (static_cast<u16>(type_size) >> 8) & 0xFF;
    fn.emit_instruction(Opcode::LoadU16Imm, *sizeof_dest, lo, hi);

    return *sizeof_dest;
}

[[nodiscard]] VAR_OPTIMIZATION_ACTION sizeof_expr::var_optimization_pass(var_optimization_env& env) noexcept {
    if (std::holds_alternative<expr_uptr>(m_operand)) {
        env.check_action(&std::get<expr_uptr>(m_operand));
    }
    return VAR_OPTIMIZATION_ACTION::NONE;
}


[[nodiscard]] FOREACH_OPTIMIZATION_ACTION sizeof_expr::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}





}