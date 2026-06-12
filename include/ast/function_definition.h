#pragma once

#include "compilation/program_binary_element.h"
#include "external_declaration.h"
#include <functional>

namespace dconstruct::ast {
    struct function_definition : public global_declaration {
        virtual void pseudo_c(ast_serialization_buffer&) const final;
        virtual void pseudo_py(ast_serialization_buffer&) const final;
        virtual void pseudo_racket(ast_serialization_buffer&) const final;
        void to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept final;

        void regex_optimization_pass() noexcept final;

        [[nodiscard]] std::vector<semantic_check_error> check_semantics(compilation::scope&) const noexcept final;
        [[nodiscard]] program_binary_result emit_dc(compilation::global_state& global) const noexcept final;

        function_definition() = default;
        function_definition(function_definition&& rhs) noexcept = default;
        function_definition& operator=(function_definition&& rhs) noexcept = default;
        function_definition(const function_definition& rhs) noexcept = delete;
        function_definition& operator=(const function_definition& rhs) noexcept = delete;


        std::vector<ast::parameter> m_parameters;
        ast::block m_body;
        ast::function_type m_type;
        function_name_variant m_name;

        void set_program_binary_element(compilation::program_binary_element element) const {
            m_cachedElement = std::move(element);
        }

    private:
        mutable std::optional<compilation::program_binary_element> m_cachedElement = std::nullopt;
    };
}
