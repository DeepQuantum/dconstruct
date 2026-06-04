#pragma once

#include "ast/statements/variable_declaration.h"
#include "ast/ast_source.h"
#include "state_script_state.h"
#include "ast/primary_expressions/sid_identifier.h"
#include "ast/external_declaration.h"
#include <string>
#include <vector>

namespace dconstruct::ast {

    struct state_script : public global_declaration {
        state_script(
            std::string name,
            std::vector<sid_identifier> options,
            std::vector<variable_declaration> declarations,
            std::vector<state_script_state> states
        ) noexcept
            : m_name(std::move(name)),
              m_options(std::move(options)),
              m_declarations(std::move(declarations)),
              m_states(std::move(states)) {}

        state_script(const state_script&) = delete;
        state_script& operator=(const state_script&) = delete;
        state_script(state_script&&) noexcept = default;
        state_script& operator=(state_script&&) noexcept = default;

        void pseudo_c(ast_serialization_buffer& buffer) const final;
        void pseudo_py(ast_serialization_buffer& buffer) const final;
        void pseudo_racket(ast_serialization_buffer& buffer) const final;
        void regex_optimization_pass() noexcept final;

        [[nodiscard]] virtual std::vector<semantic_check_error> check_semantics(compilation::scope&) const noexcept final;
        [[nodiscard]] virtual program_binary_result emit_dc(compilation::global_state& global) const noexcept final;

        [[nodiscard]] bool equals(const state_script& rhs) const noexcept;

        std::string m_name;
        std::string m_optionsString;
        std::vector<sid_identifier> m_options;
        std::vector<variable_declaration> m_declarations;
        std::vector<state_script_state> m_states;

        u64 m_initialStateIdx = 0;
    };

    [[nodiscard]] bool operator==(const state_script& lhs, const state_script& rhs) noexcept;

}
