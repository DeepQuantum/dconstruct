#pragma once

#include "ast/unary_expression.h"


namespace dconstruct::ast {
    struct negate_expr : public clonable_unary_expr<negate_expr> {
        using clonable_unary_expr::clonable_unary_expr;
        
        explicit negate_expr(expr_uptr&& rhs) noexcept : clonable_unary_expr(compiler::token{ compiler::token_type::MINUS, "-" }, std::move(rhs)) {};

        [[nodiscard]] llvm_res emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module, const compiler::scope&) const noexcept final;

        [[nodiscard]] expr_uptr simplify() const final;
        
        [[nodiscard]] full_type compute_type_unchecked(const compiler::scope&) const noexcept override;
        [[nodiscard]] semantic_check_res compute_type_checked(compiler::scope&) const noexcept override;

    };
}