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
        [[nodiscard]] virtual OPTIMIZATION_ACTION decomp_optimization_pass(optimization_pass_context& optimization_ctx) noexcept = 0;
        [[nodiscard]] virtual std::unique_ptr<statement> clone() const noexcept = 0;

        static OPTIMIZATION_ACTION check_optimization(std::unique_ptr<ast::statement>* statement, optimization_pass_context& optimization_ctx);
    };

    [[nodiscard]] inline bool operator==(const statement& lhs, const statement& rhs) noexcept {
        return lhs.equals(rhs);
    }

    [[nodiscard]] inline bool operator==(const std::unique_ptr<statement>& lhs, const std::unique_ptr<statement>& rhs) noexcept {
        return *lhs == *rhs;
    }
}

using stmnt_uptr = std::unique_ptr<dconstruct::ast::statement>;
