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

}
