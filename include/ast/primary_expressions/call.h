#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct call_expr : public expression {
        void pseudo(std::ostream &os) const final;
        void ast(std::ostream &os) const final;
    private:
        expr_uptr m_callee;
        std::vector<expr_uptr> m_arguments;
    };
}