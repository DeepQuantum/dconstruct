#pragma once

#include "ast/unary_expression.h"

namespace dconstruct::ast {
    struct logical_not_expr : public clonable_unary_expr<logical_not_expr> {
        using clonable_unary_expr::clonable_unary_expr;
        
        explicit logical_not_expr(expr_uptr&& rhs) noexcept : clonable_unary_expr(compiler::token{ compiler::token_type::BANG, "!" }, std::move(rhs)) {};

        [[nodiscard]] llvm_res emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module, const type_environment&) const noexcept final;

        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
        void pseudo_racket(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;       
    };
}