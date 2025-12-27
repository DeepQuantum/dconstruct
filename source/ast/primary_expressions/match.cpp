#include "ast/primary_expressions/match.h"


namespace dconstruct::ast {


void match_expr::pseudo_c(std::ostream& os) const {
    os << "match (";
    for (u32 i = 0; i < m_conditions.size(); ++i) {
        os << *m_conditions[i];
        if (i != m_conditions.size() - 1) {
            os << "; ";
        }
    }
    os << ") {\n";
    os << indent_more;
    for (const auto& [pattern, expression] : m_matchPairs) {
        os << indent << *pattern << " -> " << *expression << "\n";
    }
    os << indent_less << "}";
}

void match_expr::pseudo_py(std::ostream& os) const {}
void match_expr::pseudo_racket(std::ostream& os) const {}

[[nodiscard]] expr_uptr match_expr::simplify() const {
    return clone();
}

[[nodiscard]] bool match_expr::equals(const expression& rhs) const noexcept {
    const match_expr* rhs_ptr = dynamic_cast<const match_expr*>(&rhs);
    if (!rhs_ptr) {
        return false;
    }
    return m_conditions == rhs_ptr->m_conditions && m_matchPairs == rhs_ptr->m_matchPairs;
}

[[nodiscard]] u16 match_expr::calc_complexity() const noexcept {
    u16 complexity = 1;
    for (const auto& condition : m_conditions) {
        complexity += condition->get_complexity();
    }
    for (const auto& [pattern, expression] : m_matchPairs) {
        complexity += pattern->get_complexity() + expression->get_complexity();
    }
    return complexity;
}

[[nodiscard]] expr_uptr match_expr::clone() const noexcept {
    std::vector<expr_uptr> new_conditions;
    std::vector<std::tuple<expr_uptr, expr_uptr>> new_matchpairs;
    new_conditions.reserve(m_conditions.size());
    new_matchpairs.reserve(m_matchPairs.size());
    for (const auto& condition : m_conditions) {
        new_conditions.emplace_back(condition->clone());
    }
    for (const auto& [pattern, expression] : m_matchPairs) {
        new_matchpairs.emplace_back(pattern->clone(), expression->clone());
    }
    return std::make_unique<match_expr>(std::move(new_conditions), std::move(new_matchpairs));
}

VAR_OPTIMIZATION_ACTION match_expr::var_optimization_pass(var_optimization_env& env) noexcept {
    for (auto& condition : m_conditions) {
        expression::check_var_optimization(&condition, env);
    }
    for (auto& [pattern, expression] : m_matchPairs) {
        expression::check_var_optimization(&pattern, env);
        expression::check_var_optimization(&expression, env);
    }
    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION match_expr::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    for (const auto& condition : m_conditions) {
        if (const auto action = condition->foreach_optimization_pass(env); action != FOREACH_OPTIMIZATION_ACTION::NONE) {
            return action;
        }
    }
    for (const auto& [pattern, expression] : m_matchPairs) {
        if (const auto action = pattern->foreach_optimization_pass(env); action != FOREACH_OPTIMIZATION_ACTION::NONE) {
            return action;
        }
        if (const auto action = expression->foreach_optimization_pass(env); action != FOREACH_OPTIMIZATION_ACTION::NONE) {
            return action;
        }
    }
}

}