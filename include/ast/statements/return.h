#include "ast/statement.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/foreach_optimization.h"

namespace dconstruct::ast {
    struct return_stmt : public statement {
        return_stmt(expr_uptr&& expr) noexcept : m_expr(std::move(expr)) {}

        void pseudo_c(std::ostream&) const final;
        void pseudo_py(std::ostream&) const final;
        void pseudo_racket(std::ostream&) const final;
        [[nodiscard]] bool equals(const statement& rhs) const noexcept final;
        [[nodiscard]] std::unique_ptr<statement> clone() const noexcept final;
        [[nodiscard]] std::vector<semantic_check_error> check_semantics(compilation::scope& env) const noexcept final;
        [[nodiscard]] emission_err emit_dc(compilation::function& fn, compilation::global_state& global) const noexcept final;
        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;
        MATCH_OPTIMIZATION_ACTION match_optimization_pass(match_optimization_env& env) noexcept final;
        
        expr_uptr m_expr;
    };
}