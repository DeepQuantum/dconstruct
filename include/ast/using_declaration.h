#pragma once

#include "external_declaration.h"
#include "ast/primary_expressions/sid_identifier.h"

namespace dconstruct::ast {
    struct using_declaration : public global_declaration {
        explicit using_declaration(const sid_identifier name, ast::full_type new_type) noexcept : m_sidIdentifier(name), m_type{std::move(new_type)} {};

        virtual void pseudo_c(ast_serialization_buffer&) const final;
        virtual void pseudo_py(ast_serialization_buffer&) const final;
        virtual void pseudo_racket(ast_serialization_buffer&) const final;
        void to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept final;
        void regex_optimization_pass() noexcept final;

        [[nodiscard]] std::vector<semantic_check_error> check_semantics(compilation::scope&) const noexcept final;
        [[nodiscard]] resstr<compilation::program_binary_element> emit_dc(compilation::global_state& global) const noexcept final;

        sid_identifier m_sidIdentifier;
        ast::full_type m_type;
    };
}
