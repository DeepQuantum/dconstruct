#pragma once

#include "ast/expression.h"
#include "compilation/tokens.h"

namespace dconstruct::ast {
    struct identifier : public expression {
        explicit identifier(compiler::token name) : m_name(std::move(name)){};

        explicit identifier(std::string name) : m_name(compiler::token{compiler::token_type::IDENTIFIER, std::move(name), 0, 1}){};

        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
        void pseudo_racket(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] bool equals(const expression &rhs) const noexcept override;
        [[nodiscard]] expr_uptr clone() const final;
        [[nodiscard]] full_type compute_type(const type_environment& env) const final;
        [[nodiscard]] u16 calc_complexity() const noexcept final;
        [[nodiscard]] std::unique_ptr<identifier> copy() const noexcept;
        [[nodiscard]] bool identifier_name_equals(const std::string& name) const noexcept final;
        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept;
        MATCH_OPTIMIZATION_ACTION match_optimization_pass(match_optimization_env& env) noexcept;


        compiler::token m_name;
    };
}