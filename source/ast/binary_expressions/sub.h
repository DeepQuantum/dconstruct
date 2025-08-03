#pragma once

#include "expression.h"

namespace dconstruct::ast { 
    struct sub_expr : public binary_expr {
        using binary_expr::binary_expr;
        [[nodiscard]] b8 equals(const expression& rhs) const noexcept override;
        [[nodiscard]] std::unique_ptr<expression> eval() const final;

    private:
        [[nodiscard]] inline std::string get_op_char() const noexcept final { return "-"; }
        [[nodiscard]] inline std::string get_op_name() const noexcept final { return "sub"; }
    };
}