#pragma once

#include "ast/ast_source.h"
#include "ast/statements/variable_declaration.h"
#include "state.h"
#include <string>
#include <vector>

namespace dconstruct::ast {

    struct state_script : public ast_element {

        state_script(
            std::vector<sid64_literal> options,
            std::vector<variable_declaration> declarations,
            std::vector<state_script_state> states) noexcept
            : m_options(std::move(options)),
              m_declarations(std::move(declarations)),
              m_states(std::move(states)) {}

        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
        void pseudo_racket(std::ostream& os) const final;

        [[nodiscard]] bool equals(const state_script& rhs) const noexcept;

        std::vector<sid64_literal> m_options;
        std::vector<variable_declaration> m_declarations;
        std::vector<state_script_state> m_states;
    };

    [[nodiscard]] inline bool operator==(const state_script& lhs, const state_script& rhs) noexcept {
        return lhs.equals(rhs);
    }

}
