#pragma once

#include "base.h"
#include "ast/ast.h"

namespace dconstruct::ast {
    struct variable_declaration : public statement {

        explicit variable_declaration(const type_kind type, const std::string& id_name) noexcept :
        m_type(type), m_identifier(id_name), m_init(nullptr) {}; 

        explicit variable_declaration(const type_kind type, const std::string& id_name, std::unique_ptr<ast::expression>&& init) noexcept :
        m_type(type), m_identifier(id_name), m_init(std::move(init)) {}; 

        explicit variable_declaration(const type_kind type, const std::string& id_name, const ast::primitive_value& init) noexcept :
            m_type(type), m_identifier(id_name), m_init(std::make_unique<ast::literal>(init)) {
        };

        void pseudo(std::ostream& os) const final;

        void ast(std::ostream& os) const final;

        [[nodiscard]] b8 equals(const statement& rhs) const noexcept final;

        [[nodiscard]] inline const expression* get_expression_ptr() const noexcept {
            return m_init.get();
        } 

    private:
        type_kind m_type;
        std::string m_identifier;
        std::unique_ptr<expression> m_init;
    };

}