#pragma once

#include "ast/expression.h"


namespace dconstruct::ast {
    struct assign_expr : public clonable_binary_expr<assign_expr> {
        using clonable_binary_expr::clonable_binary_expr;
        [[nodiscard]] std::unique_ptr<expression> simplify() const final;
        [[nodiscard]] std::optional<ast::full_type> compute_type(const compiler::environment& env) const final;

    private:
        [[nodiscard]] std::string get_op_char() const noexcept final { return "="; }
        [[nodiscard]] std::string get_op_name() const noexcept final { return "assign"; }
    };
}