#pragma once

#include "ast/statement.h"

namespace dconstruct::ast {
    struct block : public statement {
        explicit block() noexcept : m_statements{} {};
        explicit block(std::vector<stmnt_uptr>&& stmnts) noexcept : m_statements{ std::move(stmnts) } {};
        void pseudo_c(std::ostream&) const final;
        void pseudo_py(std::ostream&) const final;
		void pseudo_racket(std::ostream&) const final;
        [[nodiscard]] bool equals(const statement& rhs) const noexcept final;

        std::vector<stmnt_uptr> m_statements;
    };
}