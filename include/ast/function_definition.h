#pragma once

#include "external_declaration.h"

namespace dconstruct::ast {
    struct function_definition : public global_declaration {
        virtual void pseudo_c(ast_serialization_buffer&) const final;
        virtual void pseudo_py(ast_serialization_buffer&) const final;
        virtual void pseudo_racket(ast_serialization_buffer&) const final;

        [[nodiscard]] std::vector<semantic_check_error> check_semantics(compilation::scope&) const noexcept final;
        [[nodiscard]] program_binary_result emit_dc(compilation::global_state& global) const noexcept final;

        function_definition() noexcept = default;
        function_definition(function_definition&& rhs) noexcept = default;
        function_definition& operator=(function_definition&& rhs) noexcept = default;

        std::vector<ast::parameter> m_parameters;
        ast::block m_body;
        ast::function_type m_type;
        function_name_variant m_name;
    };
}