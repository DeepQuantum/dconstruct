#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct identifier : public expression {
        identifier(const std::string &name, const u8 idx) : m_name(name), m_idx(idx) {};
        identifier(const u8 idx) : m_name("var_" + std::to_string(idx)), m_idx(idx) {};
        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;
        [[nodiscard]] std::unique_ptr<expression> eval() const final;
        [[nodiscard]] b8 equals(const expression &rhs) const noexcept override;
        [[nodiscard]] std::unique_ptr<expression> clone() const final;


    private:
        std::string m_name;
        u8 m_idx;        
    };
}