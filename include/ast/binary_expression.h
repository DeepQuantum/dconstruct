#pragma once
#include "expression.h"
#include "primary_expressions/grouping.h"
#include "primary_expressions/identifier.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/foreach_optimization.h"

namespace dconstruct::ast {
    template <typename impl_binary_expr>
    struct clonable_binary_expr : public binary_expr {
        using binary_expr::binary_expr;

        [[nodiscard]] std::unique_ptr<expression> clone() const final {
            auto expr = std::make_unique<impl_binary_expr>(
                m_operator,
                m_lhs ? m_lhs->clone() : nullptr,
                m_rhs ? m_rhs->clone() : nullptr
            );
            if (m_type) expr->set_type(*m_type);
            return expr;
        }

        [[nodiscard]] expr_uptr get_grouped() const {
            return std::make_unique<ast::grouping>(clone());
        }

        inline VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final {
            env.check_action(&m_lhs);
            env.check_action(&m_rhs);
            return VAR_OPTIMIZATION_ACTION::NONE;
        }

        inline FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final {
            env.check_action(&m_lhs);
            env.check_action(&m_rhs);
            return FOREACH_OPTIMIZATION_ACTION::NONE;
        }

        inline MATCH_OPTIMIZATION_ACTION match_optimization_pass(match_optimization_env& env) noexcept final {
            // if (m_operator.m_lexeme == "&&") {
            //     const auto lhs_action = m_lhs->match_optimization_pass(env);
            //     if (lhs_action == MATCH_OPTIMIZATION_ACTION::CHECK_VAR_READ) {
            //         const auto rhs_action = m_rhs->match_optimization_pass(env);
            //         if (rhs_action == MATCH_OPTIMIZATION_ACTION::MATCH_CONDITION_COMPARISON) {
            //             env.m_patternPairs.emplace_back(&)
            //         }
            //     }
            // }
            if (m_operator.m_lexeme == "==") {
                env.m_checkingCondition = true;
                const auto lhs_action = m_lhs->match_optimization_pass(env);
                env.m_checkingCondition = false;
                if (lhs_action == MATCH_OPTIMIZATION_ACTION::CHECK_VAR_SET || lhs_action == MATCH_OPTIMIZATION_ACTION::CHECK_VAR_READ) {
                    const auto rhs_action = m_rhs->match_optimization_pass(env);
                    if (rhs_action == MATCH_OPTIMIZATION_ACTION::LITERAL) {
                        env.m_checkVar = &m_lhs;
                        env.m_patterns.emplace_back(&m_rhs);
                        return MATCH_OPTIMIZATION_ACTION::MATCH_CONDITION_COMPARISON;
                    }
                }
            }
            return MATCH_OPTIMIZATION_ACTION::NONE;
        }
    };
}