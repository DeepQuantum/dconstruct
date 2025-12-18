#pragma once

#include "ast/binary_expression.h"


namespace dconstruct::ast {
    struct bitwise_or_expr : public clonable_binary_expr<bitwise_or_expr> {
        using clonable_binary_expr::clonable_binary_expr;

        explicit bitwise_or_expr(expr_uptr&& lhs, expr_uptr&& rhs) noexcept : clonable_binary_expr(compiler::token{ compiler::token_type::PIPE, "|" }, std::move(lhs), std::move(rhs)) {};
        [[nodiscard]] expected_value_ptr emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) const noexcept;
        [[nodiscard]] expr_uptr simplify() const final;
    };
}