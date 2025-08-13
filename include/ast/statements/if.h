#include "ast/statement.h"


namespace dconstruct::ast {
    struct if_stmt : public statement {
        if_stmt(std::unique_ptr<expression>&& condition,
            std::unique_ptr<statement>&& then_branch,
            std::unique_ptr<statement>&& else_branch = nullptr)
        : m_condition(std::move(condition)),
          m_then(std::move(then_branch)),
          m_else(std::move(else_branch)) {}

        void pseudo(std::ostream&) const final;
        void ast(std::ostream&) const final;
        [[nodiscard]] b8 equals(const statement& rhs) const noexcept final;

        std::unique_ptr<expression> m_condition;
        std::unique_ptr<statement> m_then;
        std::unique_ptr<statement> m_else;
    };
}