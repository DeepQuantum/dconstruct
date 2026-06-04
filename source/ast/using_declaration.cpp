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

    void using_declaration::to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept {
        buffer.append(AST_COLOR::KEYWORD, "using "sv);
        buffer.append(m_sidIdentifier);
        buffer.append(AST_COLOR::KEYWORD, " as "sv);
        type_to_colored_declaration_string(m_type, buffer);
        buffer.append(AST_COLOR::PUNCTUATION, ";"sv);
    }

    void using_declaration::regex_optimization_pass() noexcept {
        m_sidIdentifier.regex_optimization_pass();
    }

    [[nodiscard]] std::vector<semantic_check_error> using_declaration::check_semantics(compilation::scope& scope) const noexcept {
        scope.define(m_sidIdentifier.m_name.m_lexeme, m_type);
        return {};
    }

    [[nodiscard]] program_binary_result using_declaration::emit_dc(compilation::global_state& global) const noexcept {
        return compilation::program_binary_element{0};
    }

}
