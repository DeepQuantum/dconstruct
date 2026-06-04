#include "ast/expression.h"
#include "ast/primary_expressions/struct_access.h"

namespace dconstruct::ast {

    void expression::replace_if_struct_access(std::unique_ptr<expression>& expr) noexcept {
        if (std::unique_ptr<struct_access> repl = expr->to_struct_access()) {
            expr = std::move(repl);
        }
    }

    [[nodiscard]] std::unique_ptr<struct_access> unary_expr::to_struct_access() noexcept {
        replace_if_struct_access(m_rhs);
        return nullptr;
    }

    [[nodiscard]] std::unique_ptr<struct_access> binary_expr::to_struct_access() noexcept {
        replace_if_struct_access(m_lhs);
        replace_if_struct_access(m_rhs);
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
