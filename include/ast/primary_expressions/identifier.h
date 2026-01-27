#pragma once

#include "ast/expression.h"
#include "compilation/tokens.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/foreach_optimization.h"

namespace dconstruct::ast {
    struct identifier : public expression {
        explicit identifier(compiler::token name) : m_name(std::move(name)){};

        explicit identifier(std::string name) : m_name(compiler::token{compiler::token_type::IDENTIFIER, std::move(name), 0, 1}){};

        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
        void pseudo_racket(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] bool equals(const expression &rhs) const noexcept final;
        [[nodiscard]] expr_uptr clone() const final;
        [[nodiscard]] full_type compute_type_unchecked(const compiler::scope& env) const noexcept final { return std::monostate(); }
        [[nodiscard]] semantic_check_res compute_type_checked(compiler::scope& env) const noexcept final;
        [[nodiscard]] u16 calc_complexity() const noexcept final;
        [[nodiscard]] std::unique_ptr<identifier> copy() const noexcept;
        [[nodiscard]] bool identifier_name_equals(const std::string& name) const noexcept final;
        [[nodiscard]] bool is_dead_code() const noexcept final { return true; }
        [[nodiscard]] bool is_l_evaluable() const noexcept final { return true; }
        [[nodiscard]] emission_res emit_dc_lvalue(compiler::function& fn, compiler::global_state& global) const noexcept final;
        [[nodiscard]] emission_res emit_dc(compiler::function& fn, compiler::global_state& global, const std::optional<reg_idx> destination, const std::optional<u8> arg_pos) const noexcept final;

        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept;
        MATCH_OPTIMIZATION_ACTION match_optimization_pass(match_optimization_env& env) noexcept;


        compiler::token m_name;
    };
}