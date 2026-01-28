#pragma once

#include "ast/binary_expression.h"


namespace dconstruct::ast {
    struct mul_expr : public clonable_binary_expr<mul_expr> {
        using clonable_binary_expr::clonable_binary_expr;

        explicit mul_expr(expr_uptr&& lhs, expr_uptr&& rhs) noexcept : clonable_binary_expr(compilation::token{ compilation::token_type::STAR, "*" }, std::move(lhs), std::move(rhs)) {};
        [[nodiscard]] semantic_check_res compute_type_checked(compilation::scope& env) const noexcept final;
        [[nodiscard]] emission_res emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination) const noexcept final;

        [[nodiscard]] expr_uptr simplify() const final;
    };
}