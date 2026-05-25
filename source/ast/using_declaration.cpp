#include "ast/using_declaration.h"


namespace dconstruct::ast {

void using_declaration::pseudo_c(std::ostream& os) const {
    os << "using " << m_sidIdentifier << " as " << type_to_declaration_string(m_type) << ";";
}

void using_declaration::pseudo_py(std::ostream& os) const {
    os << "using " << m_sidIdentifier << " as " << type_to_declaration_string(m_type) << ";";
}

void using_declaration::pseudo_racket(std::ostream& os) const {
    os << "using " << m_sidIdentifier << " as " << type_to_declaration_string(m_type) << ";";
}

[[nodiscard]] std::vector<semantic_check_error> using_declaration::check_semantics(compilation::scope& scope) const noexcept {
    scope.define(m_sidIdentifier.m_name.m_lexeme, m_type);
    return {};
}

[[nodiscard]] program_binary_result using_declaration::emit_dc(compilation::global_state& global) const noexcept {
    return compilation::program_binary_element{0};
}


}