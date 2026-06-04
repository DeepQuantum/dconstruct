#include "ast/expression.h"
#include "ast/primary_expressions/struct_access.h"

namespace dconstruct::ast {

    [[nodiscard]] std::unique_ptr<struct_access> unary_expr::to_struct_access() noexcept {
        if (auto replacement = m_rhs->to_struct_access()) {
            m_rhs = std::move(replacement);
        }
        return nullptr;
    }

    [[nodiscard]] std::unique_ptr<struct_access> binary_expr::to_struct_access() noexcept {
        if (auto replacement = m_lhs->to_struct_access()) {
            m_lhs = std::move(replacement);
        }
        if (auto replacement = m_rhs->to_struct_access()) {
            m_rhs = std::move(replacement);
        }
        return nullptr;
    }

    void unary_expr::to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept {
        buffer.append(AST_COLOR::OPERATOR, m_operator.m_lexeme);
        buffer.append(*m_rhs);
    }

    void binary_expr::to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept {
        buffer.append(*m_lhs);
        buffer.append(AST_COLOR::OPERATOR, ' ', m_operator.m_lexeme, ' ');
        buffer.append(*m_rhs);
    }

}
