#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct cast_expr : public clonable_binary_expr<cast_expr> {
        using clonable_binary_expr::clonable_binary_expr;

        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] full_type compute_type(const compiler::environment& env) const final;
    };
}