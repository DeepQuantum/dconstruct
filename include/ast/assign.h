#pragma once

#include "ast/binary_expression.h"
#include "compilation/tokens.h"


namespace dconstruct::ast {
    struct assign_expr : public expression {
        explicit assign_expr(expr_uptr&& lhs, expr_uptr&& rhs) noexcept :
        m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {};
        
        void pseudo_c(std::ostream& os) const override;
        void pseudo_py(std::ostream& os) const override;
		void pseudo_racket(std::ostream& os) const override;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] ast::full_type compute_type(const type_environment& env) const final;
        [[nodiscard]] bool equals(const expression& rhs) const noexcept final;
        [[nodiscard]] expr_uptr clone() const final;
        [[nodiscard]] inline u16 complexity() const noexcept final;
        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;
        
        expr_uptr m_lhs;
        expr_uptr m_rhs;
    };
}