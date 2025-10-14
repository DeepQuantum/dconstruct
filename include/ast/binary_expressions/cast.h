#pragma once

#include "ast/expression.h"
#include "ast/primary_expressions/identifier.h"

namespace dconstruct::ast {
    struct cast_expr : public clonable_binary_expr<cast_expr> {
        using clonable_binary_expr::clonable_binary_expr;

        explicit cast_expr(const std::string& type_name, expr_uptr&& rhs) noexcept : 
        clonable_binary_expr(compiler::token{ compiler::token_type::IDENTIFIER, type_name }, std::make_unique<ast::identifier>(type_name), std::move(rhs)) {};


        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] full_type compute_type(const compiler::environment& env) const final;
    };
}