#include "ast/statement.h"

namespace dconstruct::ast {
    struct while_stmt : public statement {
        explicit while_stmt(expr_uptr&& condition, stmnt_uptr&& body) noexcept :
        m_condition(std::move(condition)), m_body(std::move(body)) {};

        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;

        [[nodiscard]] b8 equals(const statement& rhs) const noexcept final;

        expr_uptr m_condition;
        stmnt_uptr m_body;
    };
}