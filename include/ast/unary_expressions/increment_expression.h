#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct increment_expression : public clonable_unary_expr<increment_expression>{
        using clonable_unary_expr::clonable_unary_expr;

        explicit increment_expression(expr_uptr&& rhs) noexcept : clonable_unary_expr(compiler::token{ compiler::token_type::PLUS_PLUS, "++" }, std::move(rhs)) {};

        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;
    };
}