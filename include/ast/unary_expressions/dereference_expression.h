#pragma once

#include "ast/unary_expression.h"

namespace dconstruct::ast {
    struct dereference_expr : public clonable_unary_expr<dereference_expr>{
        using clonable_unary_expr::clonable_unary_expr;

        explicit dereference_expr(expr_uptr&& rhs) noexcept : clonable_unary_expr(compilation::token{ compilation::token_type::STAR, "*" }, std::move(rhs)) {};

        void pseudo_racket(std::ostream& os) const final;

        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] full_type compute_type_unchecked(const compilation::scope&) const noexcept override;
        [[nodiscard]] semantic_check_res compute_type_checked(compilation::scope&) const noexcept override;
        [[nodiscard]] emission_res emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> opt_destination) const noexcept final;
        [[nodiscard]] lvalue_emission_res emit_dc_lvalue(compilation::function& fn, compilation::global_state& global) const noexcept final;
        [[nodiscard]] bool is_l_evaluable() const noexcept final;
    };
}