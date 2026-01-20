#include "ast/statement.h"
#include "ast/primary_expressions/match.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/foreach_optimization.h"

namespace dconstruct::ast {
    struct if_stmt : public statement {
        if_stmt(expr_uptr&& condition,
            stmnt_uptr&& then_branch,
            stmnt_uptr&& else_branch = nullptr)
        : m_condition(std::move(condition)),
          m_then(std::move(then_branch)),
          m_else(std::move(else_branch)) {}

        void pseudo_c(std::ostream&) const final;
        void pseudo_py(std::ostream&) const final;
        void pseudo_racket(std::ostream&) const final;
        [[nodiscard]] bool equals(const statement& rhs) const noexcept final;
        [[nodiscard]] std::unique_ptr<statement> clone() const noexcept final;
        [[nodiscard]] const statement* inlineable_else_statement() const noexcept final;
        [[nodiscard]] std::vector<semantic_check_error> check_semantics(compiler::scope& env) const noexcept final;
        
        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;
        MATCH_OPTIMIZATION_ACTION match_optimization_pass(match_optimization_env& env) noexcept final;


        expr_uptr m_condition;
        stmnt_uptr m_then;
        stmnt_uptr m_else;
    };
}