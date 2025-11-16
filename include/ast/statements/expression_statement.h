#pragma once

#include "ast/statement.h"

namespace dconstruct::ast {
    struct expression_stmt : public statement {
        explicit expression_stmt(expr_uptr&& expr) noexcept : m_expression(std::move(expr)) {};
        void pseudo_c(std::ostream&) const final;
        void pseudo_py(std::ostream&) const final;
        [[nodiscard]] b8 equals(const statement& rhs) const noexcept final;
        
        expr_uptr m_expression;
    };
}