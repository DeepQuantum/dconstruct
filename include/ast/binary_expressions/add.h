#pragma once

#include "ast/binary_expression.h"


namespace dconstruct::ast {
    struct add_expr : public clonable_binary_expr<add_expr> {
        using clonable_binary_expr::clonable_binary_expr;

        explicit add_expr(expr_uptr&& lhs, expr_uptr&& rhs) noexcept : clonable_binary_expr(compilation::token{ compilation::token_type::PLUS, "+" }, std::move(lhs), std::move(rhs)) {};

        [[nodiscard]] llvm_res emit_llvm(llvm::LLVMContext&, llvm::IRBuilder<>&, llvm::Module&, const compilation::scope&) const final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] semantic_check_res compute_type_checked(compilation::scope& env) const noexcept final;
        [[nodiscard]] emission_res emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination, const std::optional<u8> arg_pos) const noexcept final;

    };
}