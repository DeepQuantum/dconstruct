#pragma once

#include "ast/expression.h"


namespace dconstruct::ast {
    struct add_expr : public clonable_binary_expr<add_expr> {
        using clonable_binary_expr::clonable_binary_expr;
        [[nodiscard]] std::unique_ptr<expression> simplify() const final;
    };
}