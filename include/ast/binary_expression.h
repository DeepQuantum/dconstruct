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

        bool decomp_optimization_pass(second_pass_env& env) noexcept {
            expression::check_optimization(&m_lhs, env);
            expression::check_optimization(&m_rhs, env);
            return false;
        }
    };
}