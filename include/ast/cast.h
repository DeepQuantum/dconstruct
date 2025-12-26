#pragma once

#include "ast/expression.h"
#include "ast/primary_expressions/identifier.h"

namespace dconstruct::ast {
    struct cast_expr : expression {
        explicit cast_expr(const ast::full_type& type, expr_uptr&& rhs) noexcept : m_type(type), m_rhs(std::move(rhs)) { };


        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
		void pseudo_racket(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] full_type compute_type(const type_environment& env) const final;
        [[nodiscard]] expr_uptr clone() const noexcept final;
        [[nodiscard]] bool equals(const expression& other) const noexcept final;
        [[nodiscard]] u16 calc_complexity() const noexcept final;
        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;


        ast::full_type m_type;
        expr_uptr m_rhs;
    };
}