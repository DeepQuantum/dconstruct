#include "ast/using_declaration.h"


namespace dconstruct::ast {

void using_declaration::pseudo_c(std::ostream& os) const {
    os << "using " << m_originalName << " as " << m_newIdentifier << ";";
}

void using_declaration::pseudo_py(std::ostream& os) const {
    os << "using " << m_originalName << " as " << m_newIdentifier << ";";
}

void using_declaration::pseudo_racket(std::ostream& os) const {
    os << "using " << m_originalName << " as " << m_newIdentifier << ";";
}

[[nodiscard]] std::vector<semantic_check_error> using_declaration::check_semantics(compilation::scope& scope) const noexcept {
    if (!m_originalName.m_name.m_lexeme.empty()) {
        scope.m_sidAliases[m_newIdentifier.m_name] = {SID(m_originalName.m_name.m_lexeme.c_str()), m_originalName.m_name.m_lexeme};
        scope.define(m_newIdentifier.m_name, m_newIdentifier.m_type);
    } else {
        scope.m_sidAliases[m_newIdentifier.m_name] = {0, m_originalName.m_name.m_lexeme};
        scope.define(m_newIdentifier.m_name, m_newIdentifier.m_type);
    }
    return {};
}

[[nodiscard]] program_binary_result using_declaration::emit_dc(compilation::global_state& global) const noexcept {
    return compilation::program_binary_element{0};
}


}