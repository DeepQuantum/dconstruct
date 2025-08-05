#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct compare_expr : public binary_expr {
        [[nodiscard]] b8 equals(const expression& rhs) const noexcept override;

        enum class comp_type {
            LT,
            LET,
            GT,
            GET,
            EQ,
            NEQ
        } m_compType;

        compare_expr(const comp_type type, std::unique_ptr<expression> lhs, std::unique_ptr<expression> rhs) 
            : binary_expr(std::move(lhs), std::move(rhs)), m_compType(type) {};

        std::unique_ptr<expression> eval() const final;
        
        [[nodiscard]] std::string get_op_char() const noexcept final { 
            switch (m_compType) {
                case comp_type::LT: return "<";
                case comp_type::LET: return "<=";
                case comp_type::GT: return ">";
                case comp_type::GET: return ">=";
                case comp_type::EQ: return "==";
                case comp_type::NEQ: return "!=";
                default: return "==";
            }
        }
        [[nodiscard]] std::string get_op_name() const noexcept final { return "comp" + get_op_char(); }
    };
}