#pragma once

#include "ast_source.h"

namespace dconstruct::ast {
    struct parameter : public ast_element {
        explicit parameter(ast::full_type type, std::string name) noexcept : m_type(std::move(type)), m_name(std::move(name)) {};

        virtual void pseudo_c(ast_serialization_buffer&) const final;
        virtual void pseudo_py(ast_serialization_buffer&) const final;
        virtual void pseudo_racket(ast_serialization_buffer&) const final;
        void to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept final;

        [[nodiscard]] bool operator==(const parameter& rhs) const noexcept {
            return m_name == rhs.m_name;
        }

        [[nodiscard]] bool operator!=(const parameter& rhs) const noexcept {
            return !(*this == rhs);
        }

        ast::full_type m_type;
        std::string m_name;
    };
}