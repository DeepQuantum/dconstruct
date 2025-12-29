#include "ast/parameter.h"


namespace dconstruct::ast {

void parameter::pseudo_c(std::ostream& os) const {
    os << ast::type_to_declaration_string(m_type) << " " << m_name;
}

void parameter::pseudo_py(std::ostream& os) const {
    os << ast::type_to_declaration_string(m_type) << " " << m_name;
}

void parameter::pseudo_racket(std::ostream& os) const {
    os << ast::type_to_declaration_string(m_type) << " " << m_name;
}

}