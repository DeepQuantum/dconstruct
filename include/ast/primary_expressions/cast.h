#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct cast_expr : public expression {
        cast_expr(full_type type, expr_uptr&& expr) : m_expr(std::move(expr)) {
            m_type = type;
        };
        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] b8 equals(const expression &rhs) const noexcept final;
        [[nodiscard]] expr_uptr clone() const final;
        [[nodiscard]] std::optional<full_type> compute_type(const compiler::environment& env) const final;
        expr_uptr m_expr;
    };
}