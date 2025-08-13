#pragma once

#include "ast/expression.h"
#include "compilation/tokens.h"


namespace dconstruct::ast {
    struct assign_expr : public expression {
        explicit assign_expr(compiler::token lhs, std::unique_ptr<expression>&& rhs) noexcept :
        m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {};
        
        void pseudo(std::ostream& os) const override;
        void ast(std::ostream& os) const override;
        [[nodiscard]] std::unique_ptr<expression> simplify() const final;
        [[nodiscard]] std::optional<ast::full_type> compute_type(const compiler::environment& env) const final;
        [[nodiscard]] b8 equals(const expression& rhs) const noexcept final;
        [[nodiscard]] std::unique_ptr<expression> clone() const final;
        compiler::token m_lhs;
        std::unique_ptr<expression> m_rhs;
    };
}