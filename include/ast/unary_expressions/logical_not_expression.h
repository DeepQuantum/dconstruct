#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct logical_not_expr : public clonable_unary_expr<logical_not_expr> {
        using clonable_unary_expr::clonable_unary_expr;
        
        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;
        [[nodiscard]] std::unique_ptr<expression> eval() const final;       
    };
}