#pragma once

#include "ast/statement.h"

namespace dconstruct::ast {
    struct for_stmt : public statement {
        explicit for_stmt(stmnt_uptr&& init, expr_uptr&& condition, expr_uptr&& incr, stmnt_uptr&& body) noexcept :
        m_init(std::move(init)), m_condition(std::move(condition)), m_incr(std::move(incr)), m_body(std::move(body)) {};

        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
        void pseudo_racket(std::ostream& os) const final;

        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;
        MATCH_OPTIMIZATION_ACTION match_optimization_pass(match_optimization_env& env) noexcept final;


        [[nodiscard]] bool equals(const statement& rhs) const noexcept final;
        [[nodiscard]] std::unique_ptr<statement> clone() const noexcept final;
        [[nodiscard]] std::vector<semantic_check_error> check_semantics(type_environment& env) const noexcept final { return {}; }


        stmnt_uptr m_init;
        expr_uptr m_condition;
        expr_uptr m_incr;
        stmnt_uptr m_body;
    };
}