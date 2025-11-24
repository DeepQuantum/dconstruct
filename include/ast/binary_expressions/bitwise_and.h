#pragma once

#include "ast/expression.h"


namespace dconstruct::ast {
    struct bitwise_and_expr : public clonable_binary_expr<bitwise_and_expr> {
        using clonable_binary_expr::clonable_binary_expr;


        explicit bitwise_and_expr(expr_uptr&& lhs, expr_uptr&& rhs) noexcept : clonable_binary_expr(compiler::token{ compiler::token_type::AMPERSAND, "&" }, std::move(lhs), std::move(rhs)) {};

        [[nodiscard]] expr_uptr simplify() const final;
    };
}