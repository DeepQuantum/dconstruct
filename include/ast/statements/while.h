#include "ast/statement.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/foreach_optimization.h"

namespace dconstruct::ast {
    struct while_stmt : public statement {
        explicit while_stmt(expr_uptr&& condition, stmnt_uptr&& body) noexcept : m_condition(std::move(condition)), m_body(std::move(body)) {};

        void pseudo_c(ast_serialization_buffer& buffer) const final;
        void pseudo_py(ast_serialization_buffer& buffer) const final;
        void pseudo_racket(ast_serialization_buffer& buffer) const final;

        [[nodiscard]] bool equals(const statement& rhs) const noexcept final;

        [[nodiscard]] std::unique_ptr<statement> clone() const noexcept final;
        [[nodiscard]] std::vector<semantic_check_error> check_semantics(compilation::scope& env) const noexcept final;
        [[nodiscard]] emission_err emit_dc(compilation::function& fn, compilation::global_state& global) const noexcept final;

        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept override;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept override;
        MATCH_OPTIMIZATION_ACTION match_optimization_pass(match_optimization_env& env) noexcept override;

        expr_uptr m_condition;
        stmnt_uptr m_body;
    };
}