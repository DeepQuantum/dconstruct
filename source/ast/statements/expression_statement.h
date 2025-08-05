#pragma once

#include "statements.h"

namespace dconstruct::ast {
    struct expression_stmnt : public statement {
        expression_stmnt(const expression* expr) noexcept : m_expression(expr) {};
        void pseudo(std::ostream&) const final;
        void ast(std::ostream&) const final;
        [[nodiscard]] b8 equals(const statement& rhs) const noexcept final;

        [[nodiscrad]] inline const expression* get_expression() const noexcept { return m_expression; }
    private:
        const expression* m_expression;
    };
}