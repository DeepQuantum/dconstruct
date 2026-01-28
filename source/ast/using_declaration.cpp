#include "ast/using_declaration.h"


namespace dconstruct::ast {

void using_declaration::pseudo_c(std::ostream& os) const {
    os << "using " << m_originalName.second << " as " << m_newIdentifier << ";";
}

void using_declaration::pseudo_py(std::ostream& os) const {
    os << "using " << m_originalName.second << " as " << m_newIdentifier << ";";
}

void using_declaration::pseudo_racket(std::ostream& os) const {
    os << "using " << m_originalName.second << " as " << m_newIdentifier << ";";
}

[[nodiscard]] std::vector<semantic_check_error> using_declaration::check_semantics(compilation::scope& scope) const noexcept {
    if (!m_originalName.second.empty()) {
        if (m_originalName.second != m_newIdentifier.m_name) {
            scope.m_sidAliases[m_newIdentifier.m_name] = m_originalName;
        } else {
            scope.m_sidAliases[m_originalName.second] = {SID(m_originalName.second.c_str()), m_originalName.second};
        }
        scope.define(m_newIdentifier.m_name, m_newIdentifier.m_type);
    } else {
        scope.m_sidAliases[m_newIdentifier.m_name] = m_originalName;
        scope.define(m_newIdentifier.m_name, m_newIdentifier.m_type);
    }
    return {};
}


}