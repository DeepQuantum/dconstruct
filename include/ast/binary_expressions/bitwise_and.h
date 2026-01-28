#pragma once

#include "ast/binary_expression.h"


namespace dconstruct::ast {
    struct bitwise_and_expr : public clonable_binary_expr<bitwise_and_expr> {
        using clonable_binary_expr::clonable_binary_expr;


        explicit bitwise_and_expr(expr_uptr&& lhs, expr_uptr&& rhs) noexcept : clonable_binary_expr(compilation::token{ compilation::token_type::AMPERSAND, "&" }, std::move(lhs), std::move(rhs)) {};
        [[nodiscard]] llvm_res emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module, const compilation::scope&) const final;
        [[nodiscard]] semantic_check_res compute_type_checked(compilation::scope& env) const noexcept final;
        [[nodiscard]] emission_res emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination, const std::optional<u8> arg_pos) const noexcept final;
        
        [[nodiscard]] expr_uptr simplify() const final;
    };
}