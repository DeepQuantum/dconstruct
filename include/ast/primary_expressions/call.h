#pragma once

#include "ast/expression.h"

namespace dconstruct::ast {
    struct call_expr : public expression {
        call_expr(compiler::token token, expr_uptr&& expr, std::vector<expr_uptr>&& args) noexcept : m_token{std::move(token)}, m_callee{std::move(expr)}, m_arguments{std::move(args)} {};
        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
        void pseudo_racket(std::ostream& os) const final;
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] bool equals(const expression &rhs) const noexcept final;
        [[nodiscard]] expr_uptr clone() const final;
        [[nodiscard]] full_type compute_type(const type_environment& env) const final;
        [[nodiscard]] u16 complexity() const noexcept final;
        [[nodiscard]] expec_llvm_value emit_llvm(llvm::LLVMContext&, llvm::IRBuilder<>&, llvm::Module&, const type_environment&) const noexcept final;
        OPTIMIZATION_ACTION decomp_optimization_pass(optimization_pass_context& optimization_ctx) noexcept final;
        compiler::token m_token;
        expr_uptr m_callee;
        std::vector<expr_uptr> m_arguments;
    };
}