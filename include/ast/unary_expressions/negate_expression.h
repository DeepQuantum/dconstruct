#pragma once

#include "ast/expression.h"


namespace dconstruct::ast {
    struct negate_expr : public clonable_unary_expr<negate_expr> {
        using clonable_unary_expr::clonable_unary_expr;
        
        explicit negate_expr(expr_uptr&& rhs) noexcept : clonable_unary_expr(compiler::token{ compiler::token_type::MINUS, "-" }, std::move(rhs)) {};

        [[nodiscard]] expec_llvm_value emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module, const type_environment&) const noexcept final;

        [[nodiscard]] expr_uptr simplify() const final;       

    };
}