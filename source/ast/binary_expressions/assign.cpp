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