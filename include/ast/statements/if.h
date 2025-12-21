#include "ast/statement.h"


namespace dconstruct::ast {
    struct if_stmt : public statement {
        if_stmt(expr_uptr&& condition,
            stmnt_uptr&& then_branch,
            stmnt_uptr&& else_branch = nullptr)
        : m_condition(std::move(condition)),
          m_then(std::move(then_branch)),
          m_else(std::move(else_branch)) {}

        void pseudo_c(std::ostream&) const final;
        void pseudo_py(std::ostream&) const final;
        void pseudo_racket(std::ostream&) const final;
        [[nodiscard]] bool equals(const statement& rhs) const noexcept final;

        bool decomp_optimization_pass(second_pass_env& env) noexcept final;

        expr_uptr m_condition;
        stmnt_uptr m_then;
        stmnt_uptr m_else;
    };
}