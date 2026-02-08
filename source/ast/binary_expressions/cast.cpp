#include "ast/cast.h"


namespace dconstruct::ast {

void cast_expr::pseudo_c(std::ostream& os) const {
    os << '(' << type_to_declaration_string(m_castType) << ')' << *m_rhs; 
}

void cast_expr::pseudo_py(std::ostream& os) const {
    os << type_to_declaration_string(m_castType) << '(' << *m_rhs << ')';
}

void cast_expr::pseudo_racket(std::ostream& os) const {
	os << type_to_declaration_string(m_castType) << "-> " << *m_rhs;  
}

[[nodiscard]] expr_uptr cast_expr::simplify() const {
    return std::make_unique<cast_expr>(m_castType, m_rhs->simplify());
}

[[nodiscard]] expr_uptr cast_expr::clone() const noexcept {
    return std::make_unique<cast_expr>(m_castType, m_rhs->clone());
}

[[nodiscard]] full_type cast_expr::compute_type_unchecked(const compilation::scope& env) const noexcept {
    return m_castType;
};

[[nodiscard]] semantic_check_res cast_expr::compute_type_checked(compilation::scope& env) const noexcept {
    semantic_check_res expr_type = m_rhs->get_type_checked(env);
    if (!expr_type) {
        return expr_type;
    }

    if (m_castType == *expr_type) {
        return m_castType;
    }

    const std::optional<std::string> invalid_cast = std::visit([](auto&& cast_type, auto&& expr_type) -> std::optional<std::string> {
        using cast_t = std::decay_t<decltype(cast_type)>;
        using expr_t = std::decay_t<decltype(expr_type)>;

        if constexpr (is_pointer<cast_t> && is_pointer<expr_t>) {
            return std::nullopt;
        } else if constexpr (is_primitive<cast_t> && is_primitive<expr_t>) {
            if (is_arithmetic(cast_type.m_type) && is_arithmetic(expr_type.m_type)) {
                return std::nullopt;
            }
            return "cannot cast between primitive types " + type_to_declaration_string(cast_type) + " and " + type_to_declaration_string(expr_type);
        } else {
            return "cannot cast expression type " + type_to_declaration_string(expr_type) + " to " + type_to_declaration_string(cast_type);
        }

    }, m_castType, *expr_type);

    if (!invalid_cast) {
        return m_castType;
    }
    return std::unexpected{semantic_check_error{*invalid_cast, this}};
}

[[nodiscard]] bool cast_expr::equals(const expression& other) const noexcept {
    return false;
}

[[nodiscard]] u16 cast_expr::calc_complexity() const noexcept {
    return 1 + m_rhs->get_complexity();
}

[[nodiscard]] expr_uptr expression::new_cast(const ast::full_type& type, const expression& expr) const noexcept { 
    return std::make_unique<cast_expr>(type, expr.get_grouped());
}

[[nodiscard]] expr_uptr cast_expr::new_cast(const ast::full_type& type, const expression&) const noexcept {
    return std::make_unique<cast_expr>(type, m_rhs->clone());
}

[[nodiscard]] bool cast_expr::is_l_evaluable() const noexcept {
    return m_rhs->is_l_evaluable();
}

[[nodiscard]] emission_res cast_expr::emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination) const noexcept {
    const emission_res expr_res = m_rhs->emit_dc(fn, global);
    if (!expr_res) {
        return expr_res;
    }

    const full_type& expr_type = *m_rhs->get_type();

    if (std::holds_alternative<ptr_type>(m_castType)) {
        return *expr_res;
    }

    assert(std::holds_alternative<primitive_type>(m_castType));
    const primitive_type cast_type = std::get<primitive_type>(m_castType);

    assert(std::holds_alternative<primitive_type>(expr_type));
    const primitive_type expr_prim_type = std::get<primitive_type>(expr_type);

    const bool cast_is_integral = is_integral(cast_type.m_type);
    const bool expr_is_integral = is_integral(expr_prim_type.m_type);
    
    if (cast_is_integral && expr_is_integral) {
        return *expr_res;
    } else if (!cast_is_integral && !expr_is_integral) {
        return *expr_res;
    }

    const emission_res cast_destination = fn.get_destination(destination);
    if (!cast_destination) {
        return cast_destination;
    }

    const Opcode cast_opcode = cast_is_integral ? Opcode::CastInteger : Opcode::CastFloat;

    fn.emit_instruction(Opcode::Move, *cast_destination, *expr_res);
    fn.emit_instruction(cast_opcode, *cast_destination);
    fn.free_register(*expr_res);
    
    return *cast_destination;
}

VAR_OPTIMIZATION_ACTION cast_expr::var_optimization_pass(var_optimization_env& env) noexcept {
    env.check_action(&m_rhs);
    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION cast_expr::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    env.check_action(&m_rhs);
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

}