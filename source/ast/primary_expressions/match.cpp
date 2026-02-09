#include "ast/primary_expressions/match.h"

#include <iomanip>

namespace dconstruct::ast {


void match_expr::pseudo_c(std::ostream& os) const {
    os << "match (";
    bool first = true;
    for (const auto& condition : m_conditions) {
        if (!first) {
            os << "; ";
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
        os << " -> " << **expression << ",\n";
    }
    os << indent << std::left << std::setw(max_size) << "else" << " -> " << *m_default << "\n";
    os << indent_less << indent << "}";
}

[[nodiscard]] std::vector<std::pair<std::vector<const expr_uptr*>, const expr_uptr*>> match_expr::group_patterns() const noexcept {
    std::vector<std::pair<std::vector<const expr_uptr*>, const expr_uptr*>> res;

    for (const auto& [patterns, match] : m_matchPairs) {
        //assert(pattern.size() == 1 && "used in decomp only so by default there should only ever be one expression per pattern");
        
        auto match_exists = [&match](const auto& pair) -> bool { return *pair.second == match; };
        
        for (const auto& pattern : patterns) {
            if (auto match_group = std::find_if(res.begin(), res.end(), match_exists); match_group != res.end()) {
            match_group->first.push_back(&pattern);
            } else {
                res.emplace_back(std::vector<const expr_uptr*>{&pattern}, &match);
            }
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
    return m_conditions == rhs_ptr->m_conditions && m_matchPairs == rhs_ptr->m_matchPairs && m_default == rhs_ptr->m_default;
}

[[nodiscard]] u16 match_expr::calc_complexity() const noexcept {
    const u16 conditions_complexity =
        std::accumulate(
            m_conditions.begin(),
            m_conditions.end(),
            u16{0},
            [](u16 acc, const auto& cond) {
                return acc + cond->get_complexity();
            }
        );

    const u16 match_pairs_complexity =
        std::accumulate(
            m_matchPairs.begin(),
            m_matchPairs.end(),
            u16{0},
            [](u16 acc, const auto& pair) {
                return acc + pair.first.size() + pair.second->get_complexity();
            }
        );

    return static_cast<u16>(1 + conditions_complexity + match_pairs_complexity);
}

[[nodiscard]] expr_uptr match_expr::clone() const {
    std::vector<expr_uptr> new_conditions;
    std::vector<matches_t> new_matchpairs;
    new_conditions.reserve(m_conditions.size());
    new_matchpairs.reserve(m_matchPairs.size());
    for (const auto& condition : m_conditions) {
        new_conditions.emplace_back(condition->clone());
    }
    for (const auto& [patterns, expression] : m_matchPairs) {
        std::vector<expr_uptr> new_patterns;
        new_patterns.reserve(patterns.size());

        for (const auto& pattern : patterns) {
            new_patterns.emplace_back(pattern->clone());
        }

        new_matchpairs.emplace_back(std::move(new_patterns), expression->clone());
    }
    return std::make_unique<match_expr>(std::move(new_conditions), std::move(new_matchpairs), m_default->clone());
}

[[nodiscard]] full_type match_expr::compute_type_unchecked(const compilation::scope& env) const noexcept {
    for (const auto& [patterns, expression] : m_matchPairs) {
        if (const auto type_res = expression->get_type_unchecked(env); !is_unknown(type_res)) {
            return type_res;
        }
    }
    return std::monostate();
}

[[nodiscard]] semantic_check_res match_expr::compute_type_checked(compilation::scope& env) const noexcept {
    for (auto cond_it = m_conditions.begin(); cond_it < m_conditions.end() - 1; ++cond_it) {
        const expr_uptr& condition = *cond_it;

        const semantic_check_res cond_type = condition->compute_type_checked(env);

        if (!cond_type) {
            return cond_type;
        }

        const std::optional<std::string> invalid_type = std::visit([](auto&& cond) -> std::optional<std::string> {
            using T = std::decay_t<decltype(cond)>;

            if constexpr (is_primitive<T>) {
                if (is_arithmetic(cond.m_type)) {
                    return std::nullopt;
                }
                return "expected arithmetic type for precondition but got " + type_to_declaration_string(cond);
            }
            return "precondition must be of arithmetic type, but got " + type_to_declaration_string(cond);
        }, *cond_type);

        if (invalid_type) {
            return std::unexpected{semantic_check_error{*invalid_type, condition.get()}};
        }
    }

    const semantic_check_res match_var_type = m_conditions.back()->get_type_checked(env);

    if (!match_var_type) {
        return match_var_type;
    } 

    const std::optional<std::string> invalid_match_var_type = std::visit([](auto&& arg) -> std::optional<std::string> {
        using match_var_t = std::decay_t<decltype(arg)>;

        if constexpr (is_primitive<match_var_t>) {
            if (arg.m_type != primitive_kind::NULLPTR && arg.m_type != primitive_kind::NOTHING) {
                return std::nullopt;
            }
            return "expected complete primitive type for match variable but got " + type_to_declaration_string(arg);
        }
        return "expected primitive type for match variable but got " + type_to_declaration_string(arg);
    }, *match_var_type);

    if (invalid_match_var_type) {
        return std::unexpected{semantic_check_error{*invalid_match_var_type, m_conditions.back().get()}};
    }

    bool result_type_set = false;
    full_type result_type = std::monostate();

    for (const auto& [patterns, expression] : m_matchPairs) {
        for (const auto& pattern : patterns) {
            const semantic_check_res pattern_type = pattern->get_type_checked(env);

            if (!pattern_type) {
                return pattern_type;
            }

            if (*pattern_type != *match_var_type) {
                return std::unexpected{semantic_check_error{
                    "expected pattern type " + type_to_declaration_string(*match_var_type) + " but got " + type_to_declaration_string(*pattern_type), pattern.get()
                }};
            }
        }

        const semantic_check_res expr_type = expression->get_type_checked(env);
        
        if (!expr_type) {
            return expr_type;
        }

        if (!result_type_set) {
            result_type = *expr_type;
            result_type_set = true;
        } else if (*expr_type != result_type) {
            return std::unexpected{semantic_check_error{"expected result type " + type_to_declaration_string(result_type) + " but got " + type_to_declaration_string(*expr_type), expression.get()}};
        }
    }

    assert(result_type_set && !std::holds_alternative<std::monostate>(result_type));

    const semantic_check_res default_type = m_default->get_type_checked(env);

    if (!default_type) {
        return default_type;
    }

    if (*default_type != result_type) {
        return std::unexpected{semantic_check_error{
            "expected default expression type " + type_to_declaration_string(result_type) + " but got " + type_to_declaration_string(*default_type), m_default.get()
        }};
    }

    return result_type;
}


VAR_OPTIMIZATION_ACTION match_expr::var_optimization_pass(var_optimization_env& env) noexcept {
    for (auto& condition : m_conditions) {
        env.check_action(&condition);
    }
    for (auto& [pattern, expression] : m_matchPairs) {
        env.check_action(&expression);
    }
    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION match_expr::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    for (auto& condition : m_conditions) {
        env.check_action(&condition);
    }
    for (auto& [pattern, expression] : m_matchPairs) {
        env.check_action(&expression);
    }
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

}