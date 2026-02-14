#pragma once

#include "ast/ast_source.h"
#include "ast/statement.h"
#include "ast/statements/block.h"
#include <memory>
#include <vector>

namespace dconstruct::ast {

    struct state_script_lambda : public ast_element {

        explicit state_script_lambda(block body) noexcept
            : m_body(std::move(body)) {}

        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
        void pseudo_racket(std::ostream& os) const final;

        [[nodiscard]] bool equals(const state_script_lambda& rhs) const noexcept;

        block m_body;
    };

    [[nodiscard]] inline bool operator==(const state_script_lambda& lhs, const state_script_lambda& rhs) noexcept {
        return lhs.equals(rhs);
    }

}
