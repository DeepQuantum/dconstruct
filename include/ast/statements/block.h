#pragma once

#include "ast/statement.h"

namespace dconstruct::ast {
    struct block : public statement {
        explicit block(std::vector<stmnt_uptr>&& stmnts) noexcept : m_statements(std::move(stmnts)) {};
        void pseudo(std::ostream&) const final;
        void ast(std::ostream&) const final;
        [[nodiscard]] b8 equals(const statement& rhs) const noexcept final;

        std::vector<stmnt_uptr> m_statements;
    };
}