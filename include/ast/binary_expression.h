#pragma once
#include "expression.h"
#include "primary_expressions/grouping.h"
#include "primary_expressions/identifier.h"

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
            if (!is_unknown(m_type)) expr->set_type(m_type);
            return expr;
        }

        [[nodiscard]] expr_uptr get_grouped() const {
            return std::make_unique<ast::grouping>(clone());
        }

        inline VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final {
            expression::check_var_optimization(&m_lhs, env);
            expression::check_var_optimization(&m_rhs, env);
            return VAR_OPTIMIZATION_ACTION::NONE;
        }

        inline FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final {
            if (const auto action = m_lhs->foreach_optimization_pass(env); action != FOREACH_OPTIMIZATION_ACTION::NONE) {
                return action;
            }
            if (const auto action = m_rhs->foreach_optimization_pass(env); action != FOREACH_OPTIMIZATION_ACTION::NONE) {
                return action;
            }
            return FOREACH_OPTIMIZATION_ACTION::NONE;
        }
    };
}