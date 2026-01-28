#pragma once

#include "ast/binary_expression.h"

namespace dconstruct::ast { 
    struct sub_expr : public clonable_binary_expr<sub_expr> {
        using clonable_binary_expr::clonable_binary_expr;

        explicit sub_expr(expr_uptr&& lhs, expr_uptr&& rhs) noexcept : clonable_binary_expr(compilation::token{ compilation::token_type::MINUS, "-" }, std::move(lhs), std::move(rhs)) {};
        [[nodiscard]] semantic_check_res compute_type_checked(compilation::scope& env) const noexcept final;
        

        [[nodiscard]] expr_uptr simplify() const final;
    };
}