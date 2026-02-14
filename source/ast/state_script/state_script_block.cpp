#include "ast/state_script/state_script_block.h"

namespace dconstruct::ast {

void state_script_block::pseudo_c(std::ostream& os) const {
    os << "block " << m_name << " {\n";
    os << indent_more;
    for (const auto& t : m_tracks) {
        os << indent << t << "\n";
    }
    os << indent_less;
    os << indent << "}";
}

void state_script_block::pseudo_py(std::ostream& os) const {
    os << "block " << m_name << ":";
    for (const auto& t : m_tracks) {
        os << "\n  " << t;
    }
}

void state_script_block::pseudo_racket(std::ostream& os) const {
    os << "(block " << m_name;
    for (const auto& t : m_tracks) {
        os << " " << t;
    }
    os << ")";
}

[[nodiscard]] bool state_script_block::equals(const state_script_block& rhs) const noexcept {
    if (m_name != rhs.m_name || m_tracks.size() != rhs.m_tracks.size()) {
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
