#include "ast/state_script/state.h"

namespace dconstruct::ast {

void state_script_state::pseudo_c(std::ostream& os) const {
    os << "state " << m_name << " {\n";
    os << indent_more;
    for (const auto& b : m_blocks) {
        os << indent << b << "\n";
    }
    os << indent_less;
    os << indent << "}";
}

void state_script_state::pseudo_py(std::ostream& os) const {
    os << "state " << m_name << ":";
    for (const auto& b : m_blocks) {
        os << "\n  " << b;
    }
}

void state_script_state::pseudo_racket(std::ostream& os) const {
    os << "(state " << m_name;
    for (const auto& b : m_blocks) {
        os << " " << b;
    }
    os << ")";
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
