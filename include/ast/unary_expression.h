
#pragma once
#include "expression.h"
#include "primary_expressions/grouping.h"
#include "primary_expressions/identifier.h"


namespace dconstruct::ast {
    template <typename impl_unary_expr>
    struct clonable_unary_expr : public unary_expr {
        using unary_expr::unary_expr;

        [[nodiscard]] std::unique_ptr<expression> clone() const final {
            auto expr = std::make_unique<impl_unary_expr>(m_operator, m_rhs != nullptr ? m_rhs->clone() : nullptr);
            if (!is_unknown(m_type)) expr->set_type(m_type);
            return expr;
        }

        inline VAR_FOLDING_ACTION decomp_optimization_pass(second_pass_env& env) noexcept override {
            expression::check_optimization(&m_rhs, env);
            return VAR_FOLDING_ACTION::NONE;
        }
    };
}