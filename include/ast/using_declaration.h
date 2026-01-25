#pragma once

#include "external_declaration.h"

namespace dconstruct::ast {
    struct using_declaration : public global_declaration {

        explicit using_declaration(const sid64_literal original, ast::full_type new_type, std::string new_name) noexcept :
        m_originalName(original), m_newIdentifier{std::move(new_type), std::move(new_name)} {};

        virtual void pseudo_c(std::ostream&) const final;
        virtual void pseudo_py(std::ostream&) const final;
        virtual void pseudo_racket(std::ostream&) const final;
        
        [[nodiscard]] std::vector<semantic_check_error> check_semantics(compiler::scope&) const noexcept final;

        sid64_literal m_originalName;
        parameter m_newIdentifier;
    };
}