#pragma once

#include "ast/binary_expression.h"


namespace dconstruct::ast {
    struct shift_expr : public clonable_binary_expr<shift_expr> {
        using clonable_binary_expr::clonable_binary_expr;

        explicit shift_expr(expr_uptr&& lhs, expr_uptr&& rhs) noexcept : clonable_binary_expr(compilation::token{ compilation::token_type::GREATER_GREATER, ">>" }, std::move(lhs), std::move(rhs)) {};


        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] semantic_check_res compute_type_checked(compilation::scope& env) const noexcept final;

    };
}