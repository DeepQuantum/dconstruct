#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct div_expr : public clonable_binary_expr<div_expr> {
    using clonable_binary_expr::clonable_binary_expr;
    [[nodiscard]] std::unique_ptr<expression> simplify() const final;

    };
}