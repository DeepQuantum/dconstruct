#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct identifier : public expression {
        identifier(const std::string &name) : m_name(name), m_idx(UINT32_MAX) {};
        identifier(const std::string &name, const u8 idx) : m_name(name), m_idx(idx) {};
        identifier(const u8 idx) : m_name("var_" + std::to_string(idx)), m_idx(idx) {};
        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;
        [[nodiscard]] std::unique_ptr<expression> simplify() const final;
        [[nodiscard]] b8 equals(const expression &rhs) const noexcept override;
        [[nodiscard]] std::unique_ptr<expression> clone() const final;
        [[nodiscard]] std::optional<full_type> compute_type(const compiler::environment& env) const final;

        [[nodiscard]] inline const std::string& get_name() const noexcept {
            return m_name;
        }
        
    private:
        std::string m_name;
        u32 m_idx;
    };
}