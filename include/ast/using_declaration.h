#pragma once

#include "external_declaration.h"
#include "ast/primary_expressions/sid_identifier.h"

namespace dconstruct::ast {
    struct using_declaration : public global_declaration {
        explicit using_declaration(const sid_identifier name, ast::full_type new_type) noexcept : m_sidIdentifier(name), m_type{std::move(new_type)} {};

        virtual void pseudo_c(ast_serialization_buffer&) const final;
        virtual void pseudo_py(ast_serialization_buffer&) const final;
        virtual void pseudo_racket(ast_serialization_buffer&) const final;

        [[nodiscard]] std::vector<semantic_check_error> check_semantics(compilation::scope&) const noexcept final;
        [[nodiscard]] program_binary_result emit_dc(compilation::global_state& global) const noexcept final;

        sid_identifier m_sidIdentifier;
        ast::full_type m_type;
    };
}