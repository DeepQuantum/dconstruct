#pragma once

#include "ast/expression.h"


namespace dconstruct::ast {
    struct add_expr : public clonable_binary_expr<add_expr> {
        using binary_expr::binary_expr;
        [[nodiscard]] std::unique_ptr<expression> eval() const final;

    private:
        [[nodiscard]] inline std::string get_op_char() const noexcept final { return "+"; }
        [[nodiscard]] inline std::string get_op_name() const noexcept final { return "add"; }
    };
}