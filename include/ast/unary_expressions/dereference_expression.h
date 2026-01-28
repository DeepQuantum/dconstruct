#pragma once

#include "ast/unary_expression.h"

namespace dconstruct::ast {
    struct dereference_expr : public clonable_unary_expr<dereference_expr>{
        using clonable_unary_expr::clonable_unary_expr;

        explicit dereference_expr(expr_uptr&& rhs) noexcept : clonable_unary_expr(compilation::token{ compilation::token_type::STAR, "*" }, std::move(rhs)) {};

        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] full_type compute_type_unchecked(const compilation::scope&) const noexcept override;
        [[nodiscard]] semantic_check_res compute_type_checked(compilation::scope&) const noexcept override;
        [[nodiscard]] bool is_l_evaluable() const noexcept final { return true; }
    };
}