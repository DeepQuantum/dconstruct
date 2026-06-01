#pragma once

#include "ast/ast_source.h"
#include "state_script_track.h"
#include <string>
#include <vector>

namespace dconstruct::ast {

    struct state_script_block : public ast_element {

        state_script_block(BLOCK_TYPE type, std::vector<state_script_track> tracks) noexcept
            : m_blockType(type), m_tracks(std::move(tracks)) {}

        state_script_block(std::string event_name, std::vector<state_script_track> tracks) noexcept
            : m_blockType(BLOCK_TYPE::EVENT), m_eventName(event_name), m_tracks(std::move(tracks)) {}

        void pseudo_c(ast_serialization_buffer& buffer) const final;
        void pseudo_py(ast_serialization_buffer& buffer) const final;
        void pseudo_racket(ast_serialization_buffer& buffer) const final;

        [[nodiscard]] bool equals(const state_script_block& rhs) const noexcept;


        BLOCK_TYPE m_blockType;
        std::optional<std::string> m_eventName = std::nullopt;
        std::vector<state_script_track> m_tracks;

        [[nodiscard]] std::string block_type_to_string() const noexcept {
            switch (m_blockType) {
                case BLOCK_TYPE::START: {
                    return "start";
                }
                case BLOCK_TYPE::END: {
                    return "end";
                }
                case BLOCK_TYPE::EVENT: {
                    assert(m_eventName.has_value());
                    return std::string("event ") + *m_eventName;
                }
                case BLOCK_TYPE::UPDATE: {
                    return "update";
                }
                case BLOCK_TYPE::VIRTUAL: {
                    return "virtual";
                }
                default: {
                    assert(false && "unreachable state");
                    return "";
                }
            }
        }
    };

    [[nodiscard]] bool operator==(const state_script_block& lhs, const state_script_block& rhs) noexcept;

}
