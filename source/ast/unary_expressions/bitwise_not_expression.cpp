#include "ast/unary_expressions/bitwise_not_expression.h"

namespace dconstruct::ast {

[[nodiscard]] expr_uptr bitwise_not_expr::simplify() const {
    return nullptr;
}

[[nodiscard]] llvm_res bitwise_not_expr::emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module, const type_environment& env) const noexcept {
    auto rhs = m_rhs->emit_llvm(ctx, builder, module, env);
    if (!rhs) {
        return rhs;
    }
    llvm::Value* res = builder.CreateXor(*rhs, -1);
    if (!res) {
        return std::unexpected{llvm_error{"xor was nullptr", *this}};
    }
    return res;
}


}