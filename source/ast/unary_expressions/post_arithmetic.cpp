#include "ast/unary_expressions/post_arithmetic.h"


namespace dconstruct::ast {

void post_arithmetic_expression::pseudo_c(std::ostream& os) const {
    os << *m_rhs << m_operator.m_lexeme;
}

void post_arithmetic_expression::pseudo_py(std::ostream& os) const {
    os << *m_rhs << m_operator.m_lexeme;
}

void post_arithmetic_expression::pseudo_racket(std::ostream& os) const {
    os << '(' << m_operator.m_lexeme << ' ' << *m_rhs << ')';
}

[[nodiscard]] expr_uptr post_arithmetic_expression::simplify() const {
    return std::make_unique<ast::post_arithmetic_expression>(m_operator, m_rhs->simplify());
}

[[nodiscard]] semantic_check_res post_arithmetic_expression::compute_type_checked(compilation::scope& env) const noexcept { 
    if (!m_rhs->is_l_evaluable()) {
        return std::unexpected{semantic_check_error{"expected lvalue for increment but got rvalue"}};
    }

    const semantic_check_res rhs_type = m_rhs->get_type_checked(env);

    if (!rhs_type) {
        return rhs_type;
    }

    const std::optional<std::string> invalid_increment = std::visit([](auto&& rhs_type) -> std::optional<std::string> {
        using T = std::decay_t<decltype(rhs_type)>;
        
        if constexpr (std::is_same_v<T, primitive_type>) {
            if (is_integral(rhs_type.m_type)) {
                return std::nullopt;
            } else {
                return "expected integral type for increment but got " + type_to_declaration_string(rhs_type);
            }
        } else {
            return "cannot increment expression with non-integral type " + type_to_declaration_string(rhs_type);
        }
    }, *rhs_type);


    if (!invalid_increment) {
        return *rhs_type;
    }

    return std::unexpected{semantic_check_error{*invalid_increment, this}};
}

[[nodiscard]] emission_res post_arithmetic_expression::emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination) const noexcept {
    const lvalue_emission_res expr_res = m_rhs->emit_dc_lvalue(fn, global);
    if (!expr_res) {
        return std::unexpected{std::move(expr_res.error())};
    }

    if (m_operator.m_type == compilation::token_type::MINUS_MINUS) {
        const emission_res tmp_reg = fn.get_next_unused_register();
        if (!tmp_reg) {
            return tmp_reg;
        }

        const u16 table_idx = fn.add_to_symbol_table(static_cast<u64>(-1));
        fn.emit_instruction(Opcode::LoadStaticI64Imm, *tmp_reg, table_idx);
        fn.emit_instruction(Opcode::IAdd, expr_res->first, expr_res->first, *tmp_reg);
        fn.free_register(*tmp_reg);
    } else {
        fn.emit_instruction(Opcode::IAddImm, expr_res->first, expr_res->first, 1);
    }

    return expr_res->first;
}



}