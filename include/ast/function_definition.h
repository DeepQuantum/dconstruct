#pragma once

#include "external_declaration.h"

namespace dconstruct::ast {
    struct function_definition : public global_declaration {
        virtual void pseudo_c(std::ostream&) const final;
        virtual void pseudo_py(std::ostream&) const final;
        virtual void pseudo_racket(std::ostream&) const final;

        [[nodiscard]] std::vector<semantic_check_error> check_semantics(compiler::scope&) const noexcept;

        std::vector<ast::parameter> m_parameters;
        ast::block m_body;
        ast::function_type m_type;
        function_name_variant m_name;
    };
}