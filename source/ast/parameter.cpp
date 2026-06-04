#include "ast/parameter.h"

namespace dconstruct::ast {

    void parameter::pseudo_c(ast_serialization_buffer& buffer) const {
        buffer.append(ast::type_to_declaration_string(m_type), ' ', m_name);
    }

    void parameter::pseudo_py(ast_serialization_buffer& buffer) const {
        buffer.append(m_name);
        const std::string type_name = ast::type_to_declaration_string(m_type);
        if (type_name != UNKNOWN_TYPE_NAME) {
            buffer.append(": "sv, type_name);
        }
    }

    void parameter::pseudo_racket(ast_serialization_buffer& buffer) const {
        buffer.append(m_name);
    }

    void parameter::to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept {
        ast::type_to_colored_declaration_string(m_type, buffer);
        buffer.append(AST_COLOR::BLANK, ' ');
        const bool is_temp_var =
            (m_name.size() >= 5 && (m_name.compare(0, 4, "var_") == 0 || m_name.compare(0, 4, "arg_") == 0)) ||
            (m_name.size() == 1 && (m_name[0] == 'i' || m_name[0] == 'j' || m_name[0] == 'k' || m_name[0] == 'l'));
        buffer.append(is_temp_var ? AST_COLOR::IDENTIFIER : AST_COLOR::MEMBER, m_name);
    }

    void parameter::regex_optimization_pass() noexcept {}

}
