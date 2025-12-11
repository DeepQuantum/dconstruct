#include "ast/binary_expressions/bitwise_and.h"


namespace dconstruct::ast {

[[nodiscard]] expr_uptr bitwise_and_expr::simplify() const { 
    return nullptr;
}

[[nodiscard]] expected_value_ptr bitwise_and_expr::emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) const {
    expected_value_ptr lhs = m_lhs->emit_llvm(ctx, builder, module);
    if (!lhs) {
        return lhs;
    }
    expected_value_ptr rhs = m_rhs->emit_llvm(ctx, builder, module);
    if (!rhs) {
        return rhs;
    }
    auto lhs_val = lhs.value();
    auto rhs_val = rhs.value();
    auto lhs_type = lhs_val->getType();
    auto rhs_type = rhs_val->getType();
    if (!lhs_type->isIntegerTy()) {
        return std::unexpected{llvm_error{"expression must have integral type", *this}};
    }
    if (!rhs_type->isIntegerTy()) {
        return std::unexpected{llvm_error{"expression must have integral type", *this}};
    }
    auto lhs_i_type_size = llvm::cast<llvm::IntegerType>(lhs_type)->getBitWidth();
    auto rhs_i_type_size = llvm::cast<llvm::IntegerType>(rhs_type)->getBitWidth();
    if (lhs_i_type_size > rhs_i_type_size) {
        lhs_val = builder.CreateZExt(lhs_val, rhs_type);
    } else if (rhs_i_type_size > lhs_i_type_size) {
        rhs_val = builder.CreateZExt(rhs_val, lhs_type);
    }
    llvm::Value* res = builder.CreateAnd(lhs_val, rhs_val);
    if (!res) {
        return std::unexpected{llvm_error{"and was nullptr", *this}};
    }
    return res;
}


}
