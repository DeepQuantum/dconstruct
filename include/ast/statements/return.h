#include "ast/statement.h"

namespace dconstruct::ast {
    struct return_stmt : public statement {
        return_stmt(expr_uptr&& expr) noexcept : m_expr(std::move(expr)) {}

        void pseudo_c(std::ostream&) const final;
        void pseudo_py(std::ostream&) const final;
        [[nodiscard]] bool equals(const statement& rhs) const noexcept final;
        
        expr_uptr m_expr;
    };
}