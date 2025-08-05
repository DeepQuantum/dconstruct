#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct call_expr : public expression {
        void pseudo(std::ostream &os) const final;
        void ast(std::ostream &os) const final;
    private:
        std::unique_ptr<expression> m_callee;
        std::vector<std::unique_ptr<expression>> m_arguments;
    };
}