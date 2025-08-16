#pragma once

#include "ast/expression.h"

namespace dconstruct::ast { 
    struct sub_expr : public clonable_binary_expr<sub_expr> {
        using clonable_binary_expr::clonable_binary_expr;
        [[nodiscard]] expr_uptr simplify() const final;
    };
}