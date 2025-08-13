#pragma once

#include "ast/expression.h"
#include "compilation/tokens.h"

namespace dconstruct::ast {
    struct identifier : public expression {
        explicit identifier(compiler::token name) : m_name(std::move(name)), m_idx(UINT32_MAX) {};

        explicit identifier(compiler::token name, const u8 idx) : m_name(std::move(name)), m_idx(idx) {};

        explicit identifier(const u32 idx) : 
        m_name(compiler::token(compiler::token_type::IDENTIFIER, "var_" + std::to_string(idx))), m_idx(idx) {};

        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;
        [[nodiscard]] std::unique_ptr<expression> simplify() const final;
        [[nodiscard]] b8 equals(const expression &rhs) const noexcept override;
        [[nodiscard]] std::unique_ptr<expression> clone() const final;
        [[nodiscard]] std::optional<full_type> compute_type(const compiler::environment& env) const final;
        
        compiler::token m_name;
        u32 m_idx;
    };
}