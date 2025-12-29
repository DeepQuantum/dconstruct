#pragma once

#include "ast_source.h"
#include "statements/block.h"
#include "parameter.h"

namespace dconstruct::ast {
    struct function_definition : ast_element {
        virtual void pseudo_c(std::ostream&) const final;
        virtual void pseudo_py(std::ostream&) const final;
        virtual void pseudo_racket(std::ostream&) const final;

        std::vector<ast::parameter> m_parameters;
        ast::block m_body;
        ast::function_type m_type;
    };
}