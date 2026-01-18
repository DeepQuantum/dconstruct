
#pragma once
#include "expression.h"
#include "primary_expressions/grouping.h"
#include "primary_expressions/identifier.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/foreach_optimization.h"

namespace dconstruct::ast {
    template <typename impl_unary_expr>
    struct clonable_unary_expr : public unary_expr {
        using unary_expr::unary_expr;

        [[nodiscard]] std::unique_ptr<expression> clone() const final {
            auto expr = std::make_unique<impl_unary_expr>(m_operator, m_rhs != nullptr ? m_rhs->clone() : nullptr);
            if (m_type) expr->set_type(*m_type);
            return expr;
        }

        inline VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept override {
            env.check_action(&m_rhs);
            return VAR_OPTIMIZATION_ACTION::NONE;
        }
        
        inline FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept override {
            env.check_action(&m_rhs);
            return FOREACH_OPTIMIZATION_ACTION::NONE;
        }
    };
}