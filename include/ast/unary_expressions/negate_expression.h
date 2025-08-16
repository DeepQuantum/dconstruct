#pragma once

#include "ast/expression.h"


namespace dconstruct::ast {
    struct negate_expression : public clonable_unary_expr<negate_expression> {
        using clonable_unary_expr::clonable_unary_expr;
        
        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;       

    };
}