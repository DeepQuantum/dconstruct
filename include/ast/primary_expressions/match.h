#pragma once

#include "ast/expression.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/foreach_optimization.h"


namespace dconstruct::ast {
    struct match_expr : public expression {
        using matches_t = std::pair<std::vector<expr_uptr>, expr_uptr>;

        match_expr(std::vector<matches_t>&& match_pairs) noexcept : 
        m_matchPairs(std::move(match_pairs)) {};

        match_expr(std::vector<matches_t>&& match_pairs, expr_uptr&& _default) noexcept : 
        m_matchPairs(std::move(match_pairs)), m_default(std::move(_default)) {};

        match_expr(expr_uptr&& condition, std::vector<matches_t>&& match_pairs, expr_uptr&& _default) noexcept : 
        m_conditions(1), m_matchPairs(std::move(match_pairs)), m_default(std::move(_default)) {
            m_conditions[0] = std::move(condition);
        };

        match_expr(std::vector<expr_uptr>&& conditions, std::vector<matches_t>&& match_pairs, expr_uptr&& _default) : 
        m_conditions(std::move(conditions)), m_matchPairs(std::move(match_pairs)), m_default(std::move(_default)) {};

        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
        void pseudo_racket(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] bool equals(const expression &rhs) const noexcept final;
        [[nodiscard]] expr_uptr clone() const final;
        [[nodiscard]] full_type compute_type(const type_environment& env) const final { return std::monostate(); };
        [[nodiscard]] u16 calc_complexity() const noexcept final;
        //[[nodiscard]] llvm_res emit_llvm(llvm::LLVMContext&, llvm::IRBuilder<>&, llvm::Module&, const type_environment&) const noexcept final {return std::};
        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;

        std::vector<expr_uptr> m_conditions;
        std::vector<matches_t> m_matchPairs;
        expr_uptr m_default;
    private:
        [[nodiscard]] std::vector<std::pair<std::vector<const expr_uptr*>, const expr_uptr*>> group_patterns() const noexcept;
    };
}