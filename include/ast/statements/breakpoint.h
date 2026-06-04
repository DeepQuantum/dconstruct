#pragma once

#include "ast/statement.h"

namespace dconstruct::ast {
    struct breakpoint : public statement {
        void pseudo_c(ast_serialization_buffer&) const final;
        void pseudo_py(ast_serialization_buffer&) const final;
        void pseudo_racket(ast_serialization_buffer&) const final;
        void to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept final;
        [[nodiscard]] bool equals(const statement& rhs) const noexcept final;
        [[nodiscard]] std::unique_ptr<statement> clone() const noexcept final;
        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        void member_access_optimization_pass() noexcept final;
        void regex_optimization_pass() noexcept final;
        [[nodiscard]] std::vector<semantic_check_error> check_semantics(compilation::scope& scope) const noexcept final;
        [[nodiscard]] emission_err emit_dc(compilation::function& fn, compilation::global_state& global) const noexcept final;
    };
}
