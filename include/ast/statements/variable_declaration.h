#pragma once

#include "base.h"
#include "ast/expression.h"
#include "ast/type.h"
#include "ast/statement.h"

namespace dconstruct::ast {
    struct variable_declaration : public statement {
        inline void pseudo(std::ostream& os) const final {
            os << m_typename << m_identifier;
            if (m_rhs != nullptr) {
                os << m_rhs;
            }
            os << ';';
        }

        inline void ast(std::ostream& os) const final {
            os << "variable_declaration[" << m_typename << ", " << m_identifier << ", ";
            if (m_rhs != nullptr) {
                os << '{' << m_rhs << '}';
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
            return m_typename == rhs_ptr->m_typename && m_identifier == rhs_ptr->m_identifier && *m_rhs == *rhs_ptr->m_rhs;
        }

    private:
        std::string m_typename;
        std::string m_identifier;
        const ast::expression* m_rhs;
    };

}