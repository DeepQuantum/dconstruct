#pragma once

#include "base.h"
#include "ast/ast.h"

namespace dconstruct::ast {
    struct variable_declaration : public statement {

        explicit variable_declaration(std::string type, std::string id_name) noexcept :
        m_typeName(std::move(type)), m_identifier(std::move(id_name)), m_init(nullptr) {}; 

        explicit variable_declaration(std::string type, std::string id_name, std::unique_ptr<ast::expression>&& init) noexcept :
        m_typeName(std::move(type)), m_identifier(std::move(id_name)), m_init(std::move(init)) {}; 

        explicit variable_declaration(std::string type, std::string id_name, const ast::primitive_value& init) noexcept :
        m_typeName(std::move(type)), m_identifier(std::move(id_name)), m_init(std::make_unique<ast::literal>(init)) {};

        void pseudo(std::ostream& os) const final;

        void ast(std::ostream& os) const final;

        [[nodiscard]] b8 equals(const statement& rhs) const noexcept final;

        [[nodiscard]] inline const expression* get_init_ptr() const noexcept {
            return m_init.get();
        } 

        std::string m_typeName;
        std::string m_identifier;
        std::unique_ptr<expression> m_init;
    };

}