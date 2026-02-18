#pragma once

#include "ast/statements/variable_declaration.h"
#include "ast/ast_source.h"
#include "state_script_state.h"
#include "ast/external_declaration.h"
#include <string>
#include <vector>

namespace dconstruct::ast {

    struct state_script : public global_declaration {

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

        [[nodiscard]] virtual std::vector<semantic_check_error> check_semantics(compilation::scope&) const noexcept final;
        [[nodiscard]] virtual program_binary_result emit_dc(compilation::global_state& global) const noexcept final { return std::unexpected("emitting state scripts isn't supported yet"); }

        [[nodiscard]] bool equals(const state_script& rhs) const noexcept;

        std::vector<sid64_literal> m_options;
        std::vector<variable_declaration> m_declarations;
        std::vector<state_script_state> m_states;
    };

    [[nodiscard]] bool operator==(const state_script& lhs, const state_script& rhs) noexcept;

}
