#pragma once

#include "ast/expression.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/foreach_optimization.h"

namespace dconstruct::ast {
    struct call_expr : public expression {
        call_expr(compiler::token token, expr_uptr&& expr, std::vector<expr_uptr>&& args) noexcept : m_token{std::move(token)}, m_callee{std::move(expr)}, m_arguments{std::move(args)} {};
        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
        void pseudo_racket(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] bool equals(const expression &rhs) const noexcept final;
        [[nodiscard]] expr_uptr clone() const final;
        [[nodiscard]] full_type compute_type(const type_environment& env) const final;
        [[nodiscard]] u16 calc_complexity() const noexcept final;
        [[nodiscard]] expr_uptr* get_first_argument() noexcept final;

        static constexpr u16 MAX_NON_SPLIT_COMPLEXITY = 10;

        [[nodiscard]] llvm_res emit_llvm(llvm::LLVMContext&, llvm::IRBuilder<>&, llvm::Module&, const type_environment&) const noexcept final;
        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;
        MATCH_OPTIMIZATION_ACTION match_optimization_pass(match_optimization_env& env) noexcept final;
        compiler::token m_token;
        expr_uptr m_callee;
        std::vector<expr_uptr> m_arguments;
    };
}