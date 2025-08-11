#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct grouping : public expression {
        grouping(std::unique_ptr<expression>&& expr) : m_expr(std::move(expr)) {};
        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;
        [[nodiscard]] std::unique_ptr<expression> simplify() const final;
        [[nodiscard]] b8 equals(const expression &rhs) const noexcept final;
        [[nodiscard]] std::unique_ptr<expression> clone() const final;
        [[nodiscard]] std::optional<full_type> compute_type(const compiler::environment& env) const final;
    private:
        std::unique_ptr<expression> m_expr;
    };
}