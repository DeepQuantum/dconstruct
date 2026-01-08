#pragma once

#include "base.h"
#include "expression.h"
#include "primary_expressions/identifier.h"
#include <vector>
#include "compilation/tokens.h"

namespace dconstruct::ast {

    struct statement : public ast_element {
        virtual ~statement() = default;
        [[nodiscard]] virtual bool equals(const statement &rhs) const noexcept = 0;
        virtual VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& optimization_env) noexcept = 0;
        virtual FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& optimization_env) noexcept { return FOREACH_OPTIMIZATION_ACTION::NONE; }
        virtual MATCH_OPTIMIZATION_ACTION match_optimization_pass(match_optimization_env& optimization_env) noexcept { return MATCH_OPTIMIZATION_ACTION::NONE; }
        [[nodiscard]] virtual std::unique_ptr<statement> clone() const noexcept = 0;
        [[nodiscard]] virtual const statement* inlineable_else_statement() const noexcept { return nullptr; }
    };

    [[nodiscard]] inline bool operator==(const statement& lhs, const statement& rhs) noexcept {
        return lhs.equals(rhs);
    }

    [[nodiscard]] inline bool operator==(const std::unique_ptr<statement>& lhs, const std::unique_ptr<statement>& rhs) noexcept {
        return *lhs == *rhs;
    }
}

using stmnt_uptr = std::unique_ptr<dconstruct::ast::statement>;
