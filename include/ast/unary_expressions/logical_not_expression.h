#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct logical_not_expr : public unary_expr {
        using unary_expr::unary_expr;
        
        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;
        [[nodiscard]] std::unique_ptr<expression> eval() const final;       
        [[nodiscard]] virtual b8 equals(const expression& other) const noexcept override;
    };
}