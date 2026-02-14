#include "ast/state_script/state_script.h"

namespace dconstruct::ast {

void state_script::pseudo_c(std::ostream& os) const {
    os << "statescript {\n";
    os << indent_more;
    os << indent << "options {\n";
    os << indent_more;
    for (const auto& opt : m_options) {
        os << indent << opt << "\n";
    }
    os << indent_less;
    os << indent << "}\n";
    os << indent << "declarations {\n";
    os << indent_more;
    for (const auto& decl : m_declarations) {
        os << indent << decl << "\n";
    }
    os << indent_less;
    os << indent << "}\n";
    for (const auto& s : m_states) {
        os << indent << s << "\n";
    }
    os << indent_less;
    os << indent << "}";
}

void state_script::pseudo_py(std::ostream& os) const {
    os << "statescript:\n  options: ";
    for (const auto& opt : m_options) {
        os << opt << " ";
    }
    os << "\n  declarations: ";
    for (const auto& decl : m_declarations) {
        os << decl << " ";
    }
    os << "\n  states: ";
    for (const auto& s : m_states) {
        os << s << " ";
    }
}

void state_script::pseudo_racket(std::ostream& os) const {
    os << "(statescript (options";
    for (const auto& opt : m_options) {
        os << " " << opt;
    }
    os << ") (declarations";
    for (const auto& decl : m_declarations) {
        os << " " << decl;
    }
    os << ") (states";
    for (const auto& s : m_states) {
        os << " " << s;
    }
    os << "))";
}

[[nodiscard]] bool state_script::equals(const state_script& rhs) const noexcept {
    if (m_options != rhs.m_options || m_declarations.size() != rhs.m_declarations.size() ||
        m_states.size() != rhs.m_states.size()) {
        return false;
    }
    for (size_t i = 0; i < m_declarations.size(); ++i) {
        if (!m_declarations[i].equals(rhs.m_declarations[i])) {
            return false;
        }
    }
    for (size_t i = 0; i < m_states.size(); ++i) {
        if (!m_states[i].equals(rhs.m_states[i])) {
            return false;
        }
    }
    return true;
}

}
