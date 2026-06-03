#include "ast/using_declaration.h"

namespace dconstruct::ast {

    void using_declaration::pseudo_c(ast_serialization_buffer& buffer) const {
        buffer.append("using "sv, m_sidIdentifier, " as "sv, type_to_declaration_string(m_type), ";"sv);
    }

    void using_declaration::pseudo_py(ast_serialization_buffer& buffer) const {
        pseudo_c(buffer);
    }

    void using_declaration::pseudo_racket(ast_serialization_buffer& buffer) const {
        pseudo_c(buffer);
    }

    [[nodiscard]] std::vector<semantic_check_error> using_declaration::check_semantics(compilation::scope& scope) const noexcept {
        scope.define(m_sidIdentifier.m_name.m_lexeme, m_type);
        return {};
    }

    [[nodiscard]] program_binary_result using_declaration::emit_dc(compilation::global_state& global) const noexcept {
        return compilation::program_binary_element{0};
    }

}
