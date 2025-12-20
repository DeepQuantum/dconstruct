#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct dereference_expr : public clonable_unary_expr<dereference_expr>{
        using clonable_unary_expr::clonable_unary_expr;

        explicit dereference_expr(expr_uptr&& rhs) noexcept : clonable_unary_expr(compiler::token{ compiler::token_type::STAR, "*" }, std::move(rhs)) {};

        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] full_type compute_type(const type_environment&) const override;
    };
}