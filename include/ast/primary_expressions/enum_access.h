#pragma once

#include "ast/primary_expressions/literal.h"
#include "compilation/tokens.h"

namespace dconstruct::ast {
    struct enum_access : public expression {
        enum_access(compilation::token token, std::string enum_name, std::string member_name, literal value) noexcept
            : m_token(std::move(token)), m_enumName(std::move(enum_name)), m_memberName(std::move(member_name)), m_value(std::move(value)) {}

        [[nodiscard]] std::string qualified_name() const;

        void pseudo_c(ast_serialization_buffer& buffer) const final;
        void pseudo_py(ast_serialization_buffer& buffer) const final;
        void pseudo_racket(ast_serialization_buffer& buffer) const final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] bool equals(const expression& other) const noexcept final;
        [[nodiscard]] expr_uptr clone() const final;
        [[nodiscard]] full_type compute_type_unchecked(const compilation::scope& env) const noexcept final;
        [[nodiscard]] u16 calc_complexity() const noexcept final;
        [[nodiscard]] const literal* as_literal() const noexcept final;
        [[nodiscard]] bool is_dead_code() const noexcept final { return true; }
        [[nodiscard]] std::unique_ptr<struct_access> to_struct_access() noexcept final;
        [[nodiscard]] semantic_check_res compute_type_checked(compilation::scope& env) const noexcept final;
        [[nodiscard]] emission_res emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination) const noexcept final;

        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;
        MATCH_OPTIMIZATION_ACTION match_optimization_pass(match_optimization_env& env) noexcept final;

        compilation::token m_token;
        std::string m_enumName;
        std::string m_memberName;
        literal m_value;
    };
}
