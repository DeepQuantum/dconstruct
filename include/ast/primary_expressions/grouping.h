#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct grouping : public expression {
        grouping(expr_uptr&& expr) : m_expr(std::move(expr)) {};
        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] b8 equals(const expression &rhs) const noexcept final;
        [[nodiscard]] expr_uptr clone() const final;
        [[nodiscard]] std::optional<full_type> compute_type(const compiler::environment& env) const final;
        [[nodiscard]] u16 complexity() const noexcept final;
    private:
        expr_uptr m_expr;
    };
}