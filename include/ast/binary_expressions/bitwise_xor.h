#pragma once

#include "ast/binary_expression.h"


namespace dconstruct::ast {
    struct bitwise_xor_expr : public clonable_binary_expr<bitwise_xor_expr> {
        using clonable_binary_expr::clonable_binary_expr;

        explicit bitwise_xor_expr(expr_uptr&& lhs, expr_uptr&& rhs) noexcept : clonable_binary_expr(compiler::token{ compiler::token_type::CARET, "^" }, std::move(lhs), std::move(rhs)) {};

        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] semantic_check_res compute_type_checked(type_environment& env) const noexcept final;

    };
}