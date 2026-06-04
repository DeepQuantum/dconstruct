#pragma once

#include "ast/expression.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/foreach_optimization.h"

namespace dconstruct::ast {
    struct match_expr : public expression {
        using matches_t = std::pair<expr_uptr, expr_uptr>;

        match_expr(std::vector<matches_t>&& match_pairs) noexcept : m_matchPairs(std::move(match_pairs)) {
            init_match_metrics();
        };

        match_expr(std::vector<matches_t>&& match_pairs, expr_uptr&& _default) noexcept : m_matchPairs(std::move(match_pairs)), m_default(std::move(_default)) {
            init_match_metrics();
        };

        match_expr(expr_uptr&& condition, std::vector<matches_t>&& match_pairs, expr_uptr&& _default) noexcept : m_conditions(1), m_matchPairs(std::move(match_pairs)), m_default(std::move(_default)) {
            m_conditions[0] = std::move(condition);
            init_match_metrics();
        };

        match_expr(std::vector<expr_uptr>&& conditions, std::vector<matches_t>&& match_pairs, expr_uptr&& _default) : m_conditions(std::move(conditions)), m_matchPairs(std::move(match_pairs)), m_default(std::move(_default)) {
            init_match_metrics();
        };

        void pseudo_c(ast_serialization_buffer& buffer) const final;
        void pseudo_py(ast_serialization_buffer& buffer) const final;
        void pseudo_racket(ast_serialization_buffer& buffer) const final;
        void to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] bool equals(const expression& rhs) const noexcept final;
        [[nodiscard]] expr_uptr clone() const final;
        [[nodiscard]] full_type compute_type_unchecked(const compilation::scope& env) const noexcept final;
        [[nodiscard]] semantic_check_res compute_type_checked(compilation::scope& env) const noexcept final;
        [[nodiscard]] u16 calc_complexity() const noexcept final;
        [[nodiscard]] std::unique_ptr<struct_access> to_struct_access() noexcept final;
        [[nodiscard]] emission_res emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination = std::nullopt) const noexcept final;

        //[[nodiscard]] llvm_res emit_llvm(llvm::LLVMContext&, llvm::IRBuilder<>&, llvm::Module&, const compiler::scope&) const noexcept final {return std::};
        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;
        void regex_optimization_pass() noexcept final;

        [[nodiscard]] const std::vector<matches_t>& get_match_pairs() const noexcept { return m_matchPairs; }

        std::vector<expr_uptr> m_conditions;
        expr_uptr m_default;

    private:
        [[nodiscard]] std::vector<std::pair<std::vector<const expr_uptr*>, const expr_uptr*>> group_patterns() const noexcept;
        void init_match_metrics() noexcept;
        void sort_matches() noexcept;
        void calc_density_R() noexcept;

        [[nodiscard]] emission_res emit_dc_array_approach(
            compilation::function& fn,
            compilation::global_state& global,
            reg_idx condition,
            const std::optional<reg_idx> destination = std::nullopt
        ) const noexcept;

        std::vector<matches_t> m_matchPairs;
        i64 m_min = 0;
        i64 m_max = 0;
        u32 m_R = 0;
        f32 m_density = 0;
        mutable std::optional<Opcode> m_loadOpcode;
        bool m_hasDensity = false;
    };
}
