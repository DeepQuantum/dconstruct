#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct bitwise_not_expr : public clonable_unary_expr<bitwise_not_expr> {
        using clonable_unary_expr::clonable_unary_expr;
        
        explicit bitwise_not_expr(expr_uptr&& rhs) noexcept : clonable_unary_expr(compiler::token{ compiler::token_type::TILDE, "~" }, std::move(rhs)) {};

        [[nodiscard]] expr_uptr simplify() const final;       
    };
}