#include "ast/statement.h"

namespace dconstruct::ast {
    struct return_stmt : public statement {
        return_stmt(expr_uptr&& expr) noexcept : m_expr(std::move(expr)) {}

        void pseudo_c(std::ostream&) const final;
        void pseudo_py(std::ostream&) const final;
        void pseudo_racket(std::ostream&) const final;
        [[nodiscard]] bool equals(const statement& rhs) const noexcept final;
        bool decomp_optimization_pass(second_pass_env& env) noexcept;
        
        expr_uptr m_expr;
    };
}