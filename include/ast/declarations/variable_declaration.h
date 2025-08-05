#pragma once

#include "base.h"
#include "printable.h"
#include "type.h"
#include "expression.h"

namespace dconstruct::ast {
    struct variable_declartion {
        ast::type_kind m_type;
        std::string m_identifier;
        const ast::expression* m_rhs;
    };
}