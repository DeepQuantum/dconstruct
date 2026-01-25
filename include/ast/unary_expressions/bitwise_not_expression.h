#pragma once

#include "ast/unary_expression.h"

namespace dconstruct::ast {
    struct bitwise_not_expr : public clonable_unary_expr<bitwise_not_expr> {
        using clonable_unary_expr::clonable_unary_expr;
        
        explicit bitwise_not_expr(expr_uptr&& rhs) noexcept : clonable_unary_expr(compiler::token{ compiler::token_type::TILDE, "~" }, std::move(rhs)) {};

        [[nodiscard]] expr_uptr simplify() const final;       
        [[nodiscard]] semantic_check_res compute_type_checked(compiler::scope& env) const noexcept final;
        [[nodiscard]] llvm_res emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module, const compiler::scope&) const noexcept final;
        //[[nodiscard]] emission_res emit_dc(compiler::function& fn, compiler::global_state& global, const bool as_argument) const noexcept final;
    };
}