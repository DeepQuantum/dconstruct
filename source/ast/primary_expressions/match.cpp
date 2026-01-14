#include "ast/primary_expressions/match.h"

#include <iomanip>

namespace dconstruct::ast {


void match_expr::pseudo_c(std::ostream& os) const {
    os << "match (";
    bool first = true;
    for (const auto& condition : m_conditions) {
        if (!first) {
            os << " ?? ";
        }
        first = false;
        os << *condition;
    }
    os << ") {\n";
    os << indent_more;

    const auto grouped = group_patterns();
    std::vector<u64> pattern_lengths;
    pattern_lengths.resize(grouped.size());

    std::transform(grouped.cbegin(), grouped.cend(), pattern_lengths.begin(), [](const auto& pair) -> u64 {
        std::stringstream os;
        for (const auto& pattern : pair.first) {
            os << **pattern;
        }
        return std::max(os.str().length() + 2 * (pair.first.size() - 1), sizeof("else") - 1);
    });

    const auto max_size_iter = std::max_element(pattern_lengths.begin(), pattern_lengths.end());

    const u64 max_size = *max_size_iter;

    for (const auto& [patterns, expression] : grouped) {
        bool first = true;
        os << indent;
        for (const auto& pattern : patterns) {
            if (!first) {
                os << ", ";
            }
            first = false;
            os << std::left << std::setw(max_size) << **pattern;
        }
        os << " -> " << **expression << "\n";
    }
    os << indent << std::left << std::setw(max_size) << "else" << " -> " << *m_default << "\n";
    os << indent_less << indent << "}";
}

[[nodiscard]] std::vector<std::pair<std::vector<const expr_uptr*>, const expr_uptr*>> match_expr::group_patterns() const noexcept {
    std::vector<std::pair<std::vector<const expr_uptr*>, const expr_uptr*>> res;

    for (const auto& [pattern, match] : m_matchPairs) {
        auto match_exists = [&match](const auto& pair) -> bool { return *pair.second == match; };
        
        if (auto match_group = std::find_if(res.begin(), res.end(), match_exists); match_group != res.end()) {
            match_group->first.push_back(&pattern);
        } else {
            res.emplace_back(std::vector<const expr_uptr*>{&pattern}, &match);
        }
    }

    return res;
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

[[nodiscard]] expr_uptr match_expr::clone() const {
    std::vector<expr_uptr> new_conditions;
    std::vector<std::pair<expr_uptr, expr_uptr>> new_matchpairs;
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
        env.check_action(&condition);
    }
    for (auto& [pattern, expression] : m_matchPairs) {
        env.check_action(&pattern);
        env.check_action(&expression);
    }
    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION match_expr::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    for (auto& condition : m_conditions) {
        env.check_action(&condition);
    }
    for (auto& [pattern, expression] : m_matchPairs) {
        env.check_action(&pattern);
        env.check_action(&expression);
    }
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

}