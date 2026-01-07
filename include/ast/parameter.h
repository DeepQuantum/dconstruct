#pragma once

#include "ast_source.h"

namespace dconstruct::ast {
    struct parameter : ast_element {
        explicit parameter(ast::full_type type, std::string name) noexcept : m_type(std::move(type)), m_name(std::move(name)) {};

        virtual void pseudo_c(std::ostream&) const final;
        virtual void pseudo_py(std::ostream&) const final;
        virtual void pseudo_racket(std::ostream&) const final;

        [[nodiscard]] bool operator==(const parameter& rhs) const noexcept {
            return m_name == rhs.m_name && m_type == rhs.m_type;
        }

        [[nodiscard]] bool operator!=(const parameter& rhs) const noexcept {
            return !(*this == rhs);
        }

        ast::full_type m_type;
        std::string m_name;
    };
}