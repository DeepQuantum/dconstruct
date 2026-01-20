#pragma once

#include "ast/binary_expression.h"
#include "compilation/tokens.h"

namespace dconstruct::ast {
    struct compare_expr : public clonable_binary_expr<compare_expr> {
        using clonable_binary_expr::clonable_binary_expr;

        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] semantic_check_res compute_type_checked(compiler::scope& env) const noexcept final;
        [[nodiscard]] full_type compute_type_unchecked(const compiler::scope&) const noexcept final;
    };
}