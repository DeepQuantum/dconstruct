#pragma once

#include "ast/expression.h"

namespace dconstruct::ast { 
    struct sub_expr : public clonable_binary_expr<sub_expr> {
        using clonable_binary_expr::clonable_binary_expr;
        [[nodiscard]] std::unique_ptr<expression> eval() const final;

    private:
        [[nodiscard]] inline std::string get_op_char() const noexcept final { return "-"; }
        [[nodiscard]] inline std::string get_op_name() const noexcept final { return "sub"; }
    };
}