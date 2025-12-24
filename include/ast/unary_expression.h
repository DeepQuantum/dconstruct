
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

        inline OPTIMIZATION_ACTION decomp_optimization_pass(optimization_pass_context& optimization_ctx) noexcept override {
            expression::check_optimization(&m_rhs, optimization_ctx);
            return OPTIMIZATION_ACTION::NONE;
        }
    };
}