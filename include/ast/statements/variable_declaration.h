#pragma once

#include "base.h"
#include "ast/ast.h"

namespace dconstruct::ast {
    struct variable_declaration : public statement {

        explicit variable_declaration(ast::full_type type, std::string id_name) noexcept :
        m_type(std::move(type)), m_identifier(std::move(id_name)), m_init(nullptr) {}; 

        explicit variable_declaration(ast::full_type type, std::string id_name, expr_uptr&& init) noexcept :
        m_type(std::move(type)), m_identifier(std::move(id_name)), m_init(std::move(init)) {}; 

        explicit variable_declaration(ast::full_type type, std::string id_name, const ast::primitive_value& init) noexcept :
        m_type(std::move(type)), m_identifier(std::move(id_name)), m_init(std::make_unique<ast::literal>(init)) {};

        void pseudo(std::ostream& os) const final;

        void ast(std::ostream& os) const final;

        [[nodiscard]] b8 equals(const statement& rhs) const noexcept final;

        [[nodiscard]] inline const expression* get_init_ptr() const noexcept {
            return m_init.get();
        } 

        ast::full_type m_type;
        std::string m_identifier;
        expr_uptr m_init;
    };

}