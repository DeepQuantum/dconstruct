#include "ast/statement.h"


namespace dconstruct::ast {
    struct if_stmt : public statement {
        if_stmt(expr_uptr&& condition,
            stmnt_uptr&& then_branch,
            stmnt_uptr&& else_branch = nullptr)
        : m_condition(std::move(condition)),
          m_then(std::move(then_branch)),
          m_else(std::move(else_branch)) {}

        void pseudo(std::ostream&) const final;
        void ast(std::ostream&) const final;
        [[nodiscard]] b8 equals(const statement& rhs) const noexcept final;

        expr_uptr m_condition;
        stmnt_uptr m_then;
        stmnt_uptr m_else;
    };
}