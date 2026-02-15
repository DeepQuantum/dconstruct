#pragma once

#include "ast/unary_expression.h"

namespace dconstruct::ast {
    struct post_arithmetic_expression : public clonable_unary_expr<post_arithmetic_expression>{
        using clonable_unary_expr::clonable_unary_expr;

        explicit post_arithmetic_expression(expr_uptr&& rhs) noexcept : clonable_unary_expr(compilation::token{ compilation::token_type::PLUS_PLUS, "++" }, std::move(rhs)) {};

        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
        void pseudo_racket(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] semantic_check_res compute_type_checked(compilation::scope& env) const noexcept final;
        [[nodiscard]] emission_res emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination) const noexcept final;
    };
}