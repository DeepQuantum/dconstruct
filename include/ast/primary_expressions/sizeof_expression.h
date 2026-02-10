#pragma once

#include "ast/expression.h"
#include "ast/optimization/var_optimization.h"


namespace dconstruct::ast {

    struct sizeof_expr : public expression {

        using operand_t = std::variant<full_type, expr_uptr>;
        
        explicit sizeof_expr(operand_t&& rhs) noexcept : m_operand(std::move(rhs)) {};

        [[nodiscard]] bool equals(const expression& rhs) const noexcept final;

        void pseudo_c(std::ostream& os) const override;

        void pseudo_py(std::ostream& os) const override;

        void pseudo_racket(std::ostream& os) const override;

        [[nodiscard]] expr_uptr clone() const noexcept final;

        [[nodiscard]] inline full_type compute_type_unchecked(const compilation::scope& env) const noexcept final;
        [[nodiscard]] virtual semantic_check_res compute_type_checked(compilation::scope& env) const noexcept final;

        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] emission_res emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination = std::nullopt) const noexcept override;
    
        [[nodiscard]] VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;

        std::variant<full_type, expr_uptr> m_operand;
    };


}