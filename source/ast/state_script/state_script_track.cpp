#include "ast/state_script/state_script_track.h"

namespace dconstruct::ast {

[[nodiscard]] bool operator==(const state_script_track& lhs, const state_script_track& rhs) noexcept {
    return lhs.equals(rhs);
}

void state_script_track::pseudo_c(std::ostream& os) const {
    os << "track " << m_name << " {\n";
    os << indent_more;
    for (const auto& lam : m_lambdas) {
        os << indent << lam << "\n";
    }
    os << indent_less;
    os << indent << "}";
}

void state_script_track::pseudo_py(std::ostream& os) const {
    os << "track " << m_name << ":";
    for (const auto& lam : m_lambdas) {
        os << "\n  lambda: " << lam;
    }
}

void state_script_track::pseudo_racket(std::ostream& os) const {
    os << "(track " << m_name;
    for (const auto& lam : m_lambdas) {
        os << " (lambda " << lam << ")";
    }
    os << ")";
}

[[nodiscard]] bool state_script_track::equals(const state_script_track& rhs) const noexcept {
    if (m_name != rhs.m_name || m_lambdas.size() != rhs.m_lambdas.size()) {
        return false;
    }
    for (size_t i = 0; i < m_lambdas.size(); ++i) {
        if (!m_lambdas[i].equals(rhs.m_lambdas[i])) {
            return false;
        }
    }
    return true;
}

}
