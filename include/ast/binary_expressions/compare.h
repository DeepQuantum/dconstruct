#pragma once

#include "ast/expression.h"
#include "compilation/tokens.h"

namespace dconstruct::ast {
    struct compare_expr : public clonable_binary_expr<compare_expr> {
        using clonable_binary_expr::clonable_binary_expr;

        [[nodiscard]] expr_uptr simplify() const final;

    };
}