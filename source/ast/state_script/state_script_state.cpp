#include "ast/state_script/state_script_state.h"

namespace dconstruct::ast {

    [[nodiscard]] bool operator==(const state_script_state& lhs, const state_script_state& rhs) noexcept {
        return lhs.equals(rhs);
    }

    void state_script_state::pseudo_c(ast_serialization_buffer& buffer) const {
        buffer.append("state "sv, m_name, " {\n"sv);
        buffer.indent_more();
        for (const auto& b : m_blocks) {
            buffer.append_indent();
            buffer.append(b, '\n');
        }
        buffer.indent_less();
        buffer.append_indent();
        buffer.append('}');
    }

    void state_script_state::pseudo_py(ast_serialization_buffer& buffer) const {
        buffer.append("state "sv, m_name, ':');
        for (const auto& b : m_blocks) {
            buffer.append("\n  "sv, b);
        }
    }

    void state_script_state::pseudo_racket(ast_serialization_buffer& buffer) const {
        buffer.append("(state "sv, m_name);
        for (const auto& b : m_blocks) {
            buffer.append(' ', b);
        }
        buffer.append(')');
    }

    [[nodiscard]] bool state_script_state::equals(const state_script_state& rhs) const noexcept {
        if (m_name != rhs.m_name || m_blocks.size() != rhs.m_blocks.size()) {
            return false;
        }
        for (size_t i = 0; i < m_blocks.size(); ++i) {
            if (!m_blocks[i].equals(rhs.m_blocks[i])) {
                return false;
            }
        }
        return true;
    }

}
