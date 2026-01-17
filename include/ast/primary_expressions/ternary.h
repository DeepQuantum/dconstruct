#pragma once

#include "ast/expression.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/foreach_optimization.h"


namespace dconstruct::ast {
    struct ternary_expr : public expression {
        ternary_expr(expr_uptr&& condition, expr_uptr&& then, expr_uptr&& _else) noexcept : 
        m_condition(std::move(condition)), m_then(std::move(then)), m_else(std::move(_else)) {};
        
        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
        void pseudo_racket(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] bool equals(const expression &rhs) const noexcept final;
        [[nodiscard]] expr_uptr clone() const final;
        [[nodiscard]] full_type compute_type_unchecked(const type_environment& env) const noexcept final;
        [[nodiscard]] semantic_check_res compute_type_checked(type_environment& env) const noexcept final;
        [[nodiscard]] u16 calc_complexity() const noexcept final;
        [[nodiscard]] llvm_res emit_llvm(llvm::LLVMContext&, llvm::IRBuilder<>&, llvm::Module&, const type_environment&) const noexcept final;
        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;

        expr_uptr m_condition;
        expr_uptr m_then;
        expr_uptr m_else;
    };
}