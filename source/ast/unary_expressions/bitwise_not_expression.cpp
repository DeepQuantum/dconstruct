#include "ast/unary_expressions/bitwise_not_expression.h"

namespace dconstruct::ast {

[[nodiscard]] expr_uptr bitwise_not_expr::simplify() const {
    return nullptr;
}

[[nodiscard]] llvm::Value* bitwise_not_expr::emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) const noexcept {
    auto res = builder.CreateXor(m_rhs->emit_llvm(ctx, builder, module), -1);
    assert(res != nullptr);
    return res;
}


}