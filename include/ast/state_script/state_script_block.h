#pragma once

#include "ast/ast_source.h"
#include "state_script_track.h"
#include <string>
#include <vector>

namespace dconstruct::ast {

    struct state_script_block : public ast_element {

        state_script_block(std::string name, std::vector<state_script_track> tracks) noexcept
            : m_name(std::move(name)), m_tracks(std::move(tracks)) {}

        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
        void pseudo_racket(std::ostream& os) const final;

        [[nodiscard]] bool equals(const state_script_block& rhs) const noexcept;

        std::string m_name;
        std::vector<state_script_track> m_tracks;
    };

    [[nodiscard]] bool operator==(const state_script_block& lhs, const state_script_block& rhs) noexcept;

}
