#pragma once

#include "ast_source.h"

namespace dconstruct::ast {
    struct parameter : ast_element {
        virtual void pseudo_c(std::ostream&) const final;
        virtual void pseudo_py(std::ostream&) const final;
        virtual void pseudo_racket(std::ostream&) const final;
        ast::full_type m_type;
        std::string m_name;
    };
}