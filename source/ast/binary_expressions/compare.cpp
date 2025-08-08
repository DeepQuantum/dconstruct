#include "ast/binary_expressions/compare.h"

namespace dconstruct::ast {

[[nodiscard]] std::unique_ptr<expression> compare_expr::eval() const {
    return nullptr;
}

[[nodiscard]] std::unique_ptr<expression> compare_expr::clone() const {
    return std::make_unique<compare_expr>(
        m_lhs != nullptr ? m_lhs->clone() : nullptr,
        m_rhs != nullptr ? m_rhs->clone() : nullptr,
        m_compType
    );
}
}