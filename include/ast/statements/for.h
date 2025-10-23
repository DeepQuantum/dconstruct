#include "ast/statement.h"

namespace dconstruct::ast {
    struct for_stmt : public statement {
        explicit for_stmt(stmnt_uptr&& init, expr_uptr&& condition, expr_uptr&& incr, stmnt_uptr&& body) noexcept :
        m_condition(std::move(condition)), m_body(std::move(body)) {};

        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;

        [[nodiscard]] b8 equals(const statement& rhs) const noexcept final;

        stmnt_uptr m_init;
        expr_uptr m_condition;
        expr_uptr m_incr;
        stmnt_uptr m_body;
    };
}