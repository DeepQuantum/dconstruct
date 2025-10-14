#pragma once

#include "ast/expression.h"


namespace dconstruct::ast {
    struct add_expr : public clonable_binary_expr<add_expr> {
        using clonable_binary_expr::clonable_binary_expr;


        explicit add_expr(expr_uptr&& lhs, expr_uptr&& rhs) noexcept : clonable_binary_expr(compiler::token{ compiler::token_type::PLUS, "+" }, std::move(lhs), std::move(rhs)) {};

        [[nodiscard]] expr_uptr simplify() const final;
    };
}