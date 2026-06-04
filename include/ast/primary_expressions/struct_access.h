#pragma once

#include "ast/expression.h"
#include "compilation/tokens.h"

namespace dconstruct::ast {
    struct struct_access : public expression {
        struct_access(expr_uptr&& lhs, compilation::token member_name) noexcept;

        void pseudo_c(ast_serialization_buffer& buffer) const final;
        void pseudo_py(ast_serialization_buffer& buffer) const final;
        void pseudo_racket(ast_serialization_buffer& buffer) const final;
        void to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] full_type compute_type_unchecked(const compilation::scope& env) const noexcept final;
        [[nodiscard]] semantic_check_res compute_type_checked(compilation::scope& env) const noexcept final;
        [[nodiscard]] expr_uptr clone() const noexcept final;
        [[nodiscard]] bool equals(const expression& other) const noexcept final;
        [[nodiscard]] u16 calc_complexity() const noexcept final;
        [[nodiscard]] bool is_l_evaluable() const noexcept final { return true; }
        [[nodiscard]] const ast::full_type* member_access_struct_type() const noexcept final;
        [[nodiscard]] std::unique_ptr<struct_access> to_struct_access() noexcept final;
        [[nodiscard]] std::optional<compilation::source_location> source_location() const noexcept final;
        [[nodiscard]] emission_res emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> opt_destination) const noexcept final;
        [[nodiscard]] lvalue_emission_res emit_dc_lvalue(compilation::function& fn, compilation::global_state& global) const noexcept final;
        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;
        MATCH_OPTIMIZATION_ACTION match_optimization_pass(match_optimization_env& env) noexcept final;

        expr_uptr m_lhs;
        compilation::token m_memberName;
    };
}
