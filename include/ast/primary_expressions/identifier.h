#pragma once

#include "ast/expression.h"
#include "compilation/tokens.h"

namespace dconstruct::ast {
    struct identifier : public expression {
        explicit identifier(compiler::token name) : m_name(std::move(name)){};

        explicit identifier(std::string name) : m_name(compiler::token{compiler::token_type::IDENTIFIER, std::move(name), 0, 1}){};

        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] b8 equals(const expression &rhs) const noexcept override;
        [[nodiscard]] expr_uptr clone() const final;
        [[nodiscard]] full_type compute_type(const compiler::environment& env) const final;
        [[nodiscard]] u16 complexity() const noexcept final;
        [[nodiscard]] std::unique_ptr<identifier> copy() const noexcept;
        
        compiler::token m_name;
    };
}