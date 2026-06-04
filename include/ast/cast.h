#pragma once

#include "ast/expression.h"
#include "ast/primary_expressions/identifier.h"
#include "ast/primary_expressions/struct_access.h"

namespace dconstruct::ast {
    struct cast_expr : public expression {
        explicit cast_expr(const ast::full_type& type, expr_uptr&& rhs) noexcept : m_castType(type), m_rhs(std::move(rhs)) {};

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
        [[nodiscard]] expr_uptr new_cast(const ast::full_type& type, const expression&) const noexcept final;
        [[nodiscard]] emission_res emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination) const noexcept final;
        [[nodiscard]] bool is_l_evaluable() const noexcept final;
        [[nodiscard]] std::unique_ptr<struct_access> to_struct_access() noexcept final;
        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;

        ast::full_type m_castType;
        expr_uptr m_rhs;

        // private:
        //  mutable std::optional<CAST_KIND> m_kind;
    };
}