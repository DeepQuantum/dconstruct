#pragma once

#include "base.h"
#include "expression.h"
#include "primary_expressions/identifier.h"
#include <vector>
#include "compilation/tokens.h"


namespace dconstruct::ast {

    struct variable_declaration;

    struct statement : public ast_element {
        virtual ~statement() = default;
        [[nodiscard]] virtual bool equals(const statement &rhs) const noexcept = 0;
        [[nodiscard]] virtual VAR_FOLDING_ACTION decomp_optimization_pass(second_pass_env& env) noexcept = 0;
        [[nodiscard]] virtual std::unique_ptr<statement> clone() const noexcept = 0;

        static void check_optimization(std::unique_ptr<ast::statement>* statement, second_pass_env& env);
    };

    [[nodiscard]] inline bool operator==(const statement& lhs, const statement& rhs) noexcept {
        return lhs.equals(rhs);
    }

    [[nodiscard]] inline bool operator==(const std::unique_ptr<statement>& lhs, const std::unique_ptr<statement>& rhs) noexcept {
        return *lhs == *rhs;
    }
}

using stmnt_uptr = std::unique_ptr<dconstruct::ast::statement>;
