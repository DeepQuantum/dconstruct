#pragma once

#include "ast/binary_expression.h"


namespace dconstruct::ast {
    struct bitwise_and_expr : public clonable_binary_expr<bitwise_and_expr> {
        using clonable_binary_expr::clonable_binary_expr;


        explicit bitwise_and_expr(expr_uptr&& lhs, expr_uptr&& rhs) noexcept : clonable_binary_expr(compiler::token{ compiler::token_type::AMPERSAND, "&" }, std::move(lhs), std::move(rhs)) {};
        [[nodiscard]] expec_llvm_value emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module, const compiler::environment&) const final;
        
        [[nodiscard]] expr_uptr simplify() const final;
    };
}