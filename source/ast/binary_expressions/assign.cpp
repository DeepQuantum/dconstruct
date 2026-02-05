#include "ast/assign.h"
#include "ast/statements/variable_declaration.h"

namespace dconstruct::ast {    

void assign_expr::pseudo_c(std::ostream& os) const {
    os << *m_lhs << " = " << *m_rhs;
}


void assign_expr::pseudo_py(std::ostream& os) const {
    os << *m_lhs << " = " << *m_rhs;
}

void assign_expr::pseudo_racket(std::ostream& os) const {
    os << "(set! " << *m_lhs << ' ' << *m_rhs << ')';
}

[[nodiscard]] expr_uptr assign_expr::simplify() const {
    return m_rhs->simplify();
}

// [[nodiscard]] ast::full_type assign_expr::compute_type(const compiler::scope& env) const {
//     return m_rhs->compute_type(env);
// }


[[nodiscard]] bool assign_expr::equals(const expression& rhs) const noexcept {
    const assign_expr* rhs_ptr = dynamic_cast<const assign_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_lhs == rhs_ptr->m_lhs && m_rhs == rhs_ptr->m_rhs;
}

[[nodiscard]] expr_uptr assign_expr::clone() const {
    return std::make_unique<assign_expr>(m_lhs->clone(), m_rhs != nullptr ? m_rhs->clone() : nullptr);
}

[[nodiscard]] u16 assign_expr::calc_complexity() const noexcept {
    return 1 + m_rhs->get_complexity();
}

[[nodiscard]] full_type assign_expr::compute_type_unchecked(const compilation::scope& scope) const noexcept {
    return m_rhs->compute_type_unchecked(scope);
}

[[nodiscard]] semantic_check_res assign_expr::compute_type_checked(compilation::scope& scope) const noexcept {
    const semantic_check_res lhs_type = m_lhs->get_type_checked(scope);
    if (!lhs_type) {
        return lhs_type;
    }

    const semantic_check_res rhs_type = m_rhs->get_type_checked(scope);
    if (!rhs_type) {
        return rhs_type;
    }

    if (!m_lhs->is_l_evaluable()) {
        return std::unexpected{semantic_check_error{"cannot assign to rvalue", m_lhs.get()}};
    }

    std::optional<std::string> assignable_err = not_assignable_reason(*lhs_type, *rhs_type);

    if (assignable_err) {
        return std::unexpected{semantic_check_error{std::move(*assignable_err)}};
    }
    return *rhs_type;
}

[[nodiscard]] emission_res assign_expr::emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination) const noexcept {
    const lvalue_emission_res lvalue = m_lhs->emit_dc_lvalue(fn, global);
    if (!lvalue) {
        return std::unexpected{std::move(lvalue.error())};
    }
    const auto& [lvalue_reg, opcode] = *lvalue;
    const emission_res rvalue = m_rhs->emit_dc(fn, global, lvalue_reg);
    if (!rvalue) {
        return rvalue;
    }
    if (*rvalue != lvalue_reg) {
        fn.emit_instruction(opcode, lvalue_reg, *rvalue);
    }
    return lvalue_reg;
}

VAR_OPTIMIZATION_ACTION assign_expr::var_optimization_pass(var_optimization_env& env)  noexcept {
    env.check_action(&m_rhs);
    const auto* lhs = dynamic_cast<const identifier*>(m_lhs.get());
    if (!lhs || !lhs->m_name.m_lexeme.starts_with("var")) {
        return VAR_OPTIMIZATION_ACTION::NONE;
    }
    if (dynamic_cast<const identifier*>(m_rhs.get())) {
        if (env.m_env.lookup(lhs->m_name.m_lexeme)) {
            return VAR_OPTIMIZATION_ACTION::NONE;
        }
    }
    return VAR_OPTIMIZATION_ACTION::VAR_WRITE;
}

FOREACH_OPTIMIZATION_ACTION assign_expr::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    env.check_action(&m_lhs);
    env.check_action(&m_rhs);
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

MATCH_OPTIMIZATION_ACTION assign_expr::match_optimization_pass(match_optimization_env& env) noexcept {
    if (env.m_resultDeclaration) {
        const auto& assign = static_cast<ast::variable_declaration&>(**env.m_resultDeclaration).m_identifier;
        if (m_lhs->match_optimization_pass(env) == MATCH_OPTIMIZATION_ACTION::RESULT_VAR_WRITE) {
            const auto& id = static_cast<ast::identifier&>(*m_lhs);
            if (id.m_name.m_lexeme == assign) {
                env.m_matches.push_back(&m_rhs);
                return MATCH_OPTIMIZATION_ACTION::RESULT_VAR_ASSIGNMENT;
            }
        }
    }
    return MATCH_OPTIMIZATION_ACTION::NONE;
}

}