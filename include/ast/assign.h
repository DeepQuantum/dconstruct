#pragma once

#include "ast/binary_expression.h"
#include "compilation/tokens.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/foreach_optimization.h"

namespace dconstruct::ast {
    struct assign_expr : public expression {
        explicit assign_expr(expr_uptr&& lhs, expr_uptr&& rhs) noexcept :
        m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {};
        
        void pseudo_c(std::ostream& os) const override;
        void pseudo_py(std::ostream& os) const override;
		void pseudo_racket(std::ostream& os) const override;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] full_type compute_type_unchecked(const compiler::scope& env) const noexcept final { return std::monostate(); }
        [[nodiscard]] semantic_check_res compute_type_checked(compiler::scope& env) const noexcept final { return std::monostate(); }
        [[nodiscard]] bool equals(const expression& rhs) const noexcept final;
        [[nodiscard]] expr_uptr clone() const final;
        [[nodiscard]] u16 calc_complexity() const noexcept final;
        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;
        MATCH_OPTIMIZATION_ACTION match_optimization_pass(match_optimization_env& env) noexcept final;
        
        expr_uptr m_lhs;
        expr_uptr m_rhs;
    };
}