#include "ast/state_script/state_script_block.h"

namespace dconstruct::ast {

[[nodiscard]] bool operator==(const state_script_block& lhs, const state_script_block& rhs) noexcept {
    return lhs.equals(rhs);
}

void state_script_block::pseudo_c(ast_serialization_buffer& buffer) const {
    buffer.append("block "sv, block_type_to_string(), " {\n"sv);
    buffer.indent_more();
    for (const auto& t : m_tracks) {
        buffer.append_indent();
        buffer.append(t, '\n');
    }
    buffer.indent_less();
    buffer.append_indent();
    buffer.append('}');
}

void state_script_block::pseudo_py(ast_serialization_buffer& buffer) const {
    buffer.append("block "sv, block_type_to_string(), ':');
    for (const auto& t : m_tracks) {
        buffer.append("\n  "sv, t);
    }
}

void state_script_block::pseudo_racket(ast_serialization_buffer& buffer) const {
    buffer.append("(block "sv, block_type_to_string());
    for (const auto& t : m_tracks) {
        buffer.append(' ', t);
    }
    buffer.append(')');
}

[[nodiscard]] bool state_script_block::equals(const state_script_block& rhs) const noexcept {
    if (block_type_to_string() != rhs.block_type_to_string() || m_tracks.size() != rhs.m_tracks.size()) {
        return false;
    }
    for (size_t i = 0; i < m_tracks.size(); ++i) {
        if (!m_tracks[i].equals(rhs.m_tracks[i])) {
            return false;
        }
    }
    return true;
}


}
