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

}
