#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct call_expr : public expression {
        call_expr(compiler::token token, expr_uptr&& expr, std::vector<expr_uptr>&& args) noexcept : m_token{std::move(token)}, m_callee{std::move(expr)}, m_arguments{std::move(args)} {};
        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] b8 equals(const expression &rhs) const noexcept final;
        [[nodiscard]] expr_uptr clone() const final;
        [[nodiscard]] std::optional<full_type> compute_type(const compiler::environment& env) const final;
        [[nodiscard]] u16 complexity() const noexcept final;
    private:
        compiler::token m_token;
        expr_uptr m_callee;
        std::vector<expr_uptr> m_arguments;
    };
}