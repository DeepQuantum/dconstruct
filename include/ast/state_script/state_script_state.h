#pragma once

#include "ast/ast_source.h"
#include "state_script_block.h"
#include <string>
#include <vector>

namespace dconstruct::ast {

    struct state_script_state : public ast_element {

        state_script_state(std::string name, std::vector<state_script_block> blocks) noexcept
            : m_name(std::move(name)), m_blocks(std::move(blocks)) {}

        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
        void pseudo_racket(std::ostream& os) const final;

        [[nodiscard]] bool equals(const state_script_state& rhs) const noexcept;

        std::string m_name;
        std::vector<state_script_block> m_blocks;
    };

    [[nodiscard]] bool operator==(const state_script_state& lhs, const state_script_state& rhs) noexcept;

}
