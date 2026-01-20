#pragma once

#include "ast/statement.h"
#include "ast/parameter.h"

namespace dconstruct::ast {
    struct foreach_stmt : public statement {
        explicit foreach_stmt(parameter var, expr_uptr&& iterable, stmnt_uptr&& body) noexcept :
        m_var(std::move(var)), m_iterable(std::move(iterable)), m_body(std::move(body)) {};

        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
        void pseudo_racket(std::ostream& os) const final;

        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;
        MATCH_OPTIMIZATION_ACTION match_optimization_pass(match_optimization_env& env) noexcept final;
        [[nodiscard]] bool equals(const statement& rhs) const noexcept final;
        [[nodiscard]] std::unique_ptr<statement> clone() const noexcept final;
        [[nodiscard]] std::vector<semantic_check_error> check_semantics(compiler::scope& env) const noexcept final { return {}; }


        parameter m_var;
        expr_uptr m_iterable;
        stmnt_uptr m_body;
    };
}