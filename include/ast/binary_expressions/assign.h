#pragma once

#include "ast/expression.h"


namespace dconstruct::ast {
    struct assign_expr : public clonable_binary_expr<assign_expr>, public comparable_binary_expr<assign_expr> {
        using binary_expr::binary_expr;
        [[nodiscard]] std::unique_ptr<expression> eval() const final;

    private:
        [[nodiscard]] std::string get_op_char() const noexcept final { return "="; }
        [[nodiscard]] std::string get_op_name() const noexcept final { return "assign"; }
    };
}