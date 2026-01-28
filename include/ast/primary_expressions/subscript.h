#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct subscript_expr : public expression {

        explicit subscript_expr(expr_uptr&& lhs, expr_uptr&& rhs) noexcept : m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) { };


        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
		void pseudo_racket(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] full_type compute_type_unchecked(const compilation::scope& env) const noexcept final;
        [[nodiscard]] semantic_check_res compute_type_checked(compilation::scope& env) const noexcept final;
        [[nodiscard]] expr_uptr clone() const noexcept final;   
        [[nodiscard]] bool equals(const expression& other) const noexcept final;
        [[nodiscard]] u16 calc_complexity() const noexcept final;
        [[nodiscard]] bool is_l_evaluable() const noexcept final { return true; }
        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;

        expr_uptr m_lhs;
        expr_uptr m_rhs;
    };
}