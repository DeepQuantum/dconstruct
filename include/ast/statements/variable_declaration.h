#pragma once

#include "base.h"
#include "ast/ast.h"

namespace dconstruct::ast {
    struct variable_declaration : public statement {

        explicit variable_declaration(const type_kind type, const std::string& id_name) noexcept :
        m_type(type), m_identifier(id_name) {}; 

        explicit variable_declaration(const type_kind type, const std::string& id_name, std::unique_ptr<ast::expression> init) noexcept :
        m_type(type), m_identifier(id_name), m_init(std::move(init)) {}; 

        inline void pseudo(std::ostream& os) const final {
            os << kind_to_string(m_type) << m_identifier;
            if (m_init != nullptr) {
                os << m_init;
            }
            os << ';';
        }

        inline void ast(std::ostream& os) const final {
            os << "variable_declaration[" << kind_to_string(m_type) << ", " << m_identifier << ", ";
            if (m_init != nullptr) {
                os << '{' << m_init << '}';
            } else {
                os << "null";
            }
            os << ';';
        }

        [[nodiscard]] inline b8 equals(const statement& rhs) const noexcept final {
            const variable_declaration* rhs_ptr = dynamic_cast<const variable_declaration*>(&rhs); 
            if (rhs_ptr == nullptr) { 
                return false; 
            }
            return m_type == rhs_ptr->m_type && m_identifier == rhs_ptr->m_identifier && *m_init == *rhs_ptr->m_init;
        }

    private:
        type_kind m_type;
        std::string m_identifier;
        std::unique_ptr<expression> m_init;
    };

}