#pragma once

#include "ast/expression.h"
#include "ast/type.h"

namespace dconstruct::ast {

    struct literal : public expression {
        template <typename T>
        literal(T&& value) : m_value(std::forward<T>(value)) {};

        inline static u32 m_emittedStringCount = 0;

        // [[nodiscard]] llvm_res emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module, const compilation::scope&) const noexcept final;

        [[nodiscard]] inline primitive_kind get_type() const noexcept {
            return kind_from_primitive_value(m_value);
        }

        void pseudo_c(ast_serialization_buffer& buffer) const final;

        void pseudo_py(ast_serialization_buffer& buffer) const final;

        void pseudo_racket(ast_serialization_buffer& buffer) const final;

        void to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept final;

        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] bool equals(const expression& other) const noexcept final;
        [[nodiscard]] expr_uptr clone() const final;
        [[nodiscard]] full_type compute_type_unchecked(const compilation::scope& env) const noexcept final;
        [[nodiscard]] u16 calc_complexity() const noexcept final;
        [[nodiscard]] const literal* as_literal() const noexcept final;
        [[nodiscard]] bool is_dead_code() const noexcept final { return true; }
        [[nodiscard]] std::unique_ptr<struct_access> to_struct_access() noexcept final;
        [[nodiscard]] semantic_check_res compute_type_checked(compilation::scope& env) const noexcept final;
        [[nodiscard]] std::optional<i64> raw_pattern_number() const noexcept final;
        [[nodiscard]] std::expected<u16, std::string> emit_to_symbol_table(compilation::function& fn, compilation::global_state& global) const noexcept final;
        [[nodiscard]] emission_res emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination) const noexcept final;

        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;
        MATCH_OPTIMIZATION_ACTION match_optimization_pass(match_optimization_env& env) noexcept final;

        primitive_value m_value;
    };
}
