#include "ast/function_definition.h"

namespace dconstruct::ast {

void function_definition::pseudo_c(std::ostream& os) const {
    os << type_to_declaration_string(*m_type.m_return) << " " << m_name << "(";
    bool first = true;
    for (const auto& param : m_parameters) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << param;
    } 
    os << ") " << m_body;
}

void function_definition::pseudo_py(std::ostream& os) const {

}

void function_definition::pseudo_racket(std::ostream& os) const {
}

}