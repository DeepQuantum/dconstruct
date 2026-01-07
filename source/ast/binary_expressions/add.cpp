#include "ast/binary_expressions/add.h"
#include "ast/primary_expressions/literal.h"

namespace dconstruct::ast {

[[nodiscard]] expr_uptr add_expr::simplify() const {
    expr_uptr lhs_ptr = m_lhs->simplify();
    expr_uptr rhs_ptr = m_rhs->simplify();

    if (const literal* lhs_num_lit = dynamic_cast<const literal*>(lhs_ptr.get())) {
        if (const literal* rhs_num_lit = dynamic_cast<const literal*>(rhs_ptr.get())) {
            // int a = 0;
            // sid b = #ellie;
            if (*lhs_num_lit != *rhs_num_lit) {
                return nullptr;
            }
            //if (lhs_num_lit)
        }
    }
    return std::make_unique<add_expr>(m_operator, std::move(lhs_ptr), std::move(rhs_ptr));
}

[[nodiscard]] llvm_res add_expr::emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module, const type_environment& env) const {
    llvm_res lhs = m_lhs->emit_llvm(ctx, builder, module, env);
    if (!lhs) {
        return lhs;
    }
    llvm_res rhs = m_rhs->emit_llvm(ctx, builder, module, env); 
    if (!rhs) {
        return rhs;
    }
    llvm::Value* lhs_val = *lhs;
    llvm::Value* rhs_val = *rhs;
    llvm::Type* lhs_type = lhs_val->getType();
    llvm::Type* rhs_type = rhs_val->getType();
    
    assert(lhs_type != nullptr);
    assert(rhs_type != nullptr);
    
    llvm::Value* res = nullptr;

    if (lhs_type->isPointerTy()) {
        if (!rhs_type->isIntegerTy()) {
            return std::unexpected{llvm_error{"rhs of ptr add must be integral type", *this}};
        }
        res = builder.CreatePtrAdd(lhs_val, rhs_val);
    } else if (rhs_type->isPointerTy()) {
        if (!lhs_type->isIntegerTy()) {
            return std::unexpected{llvm_error{"lhs of ptr add must be integral type", *this}};
        }
        res = builder.CreatePtrAdd(rhs_val, lhs_val);
    } else if (lhs_type->isFloatingPointTy()) {
        if (!rhs_type->isFloatingPointTy()) {
            const auto cast_op = is_signed(m_rhs->get_type(env)) ? llvm::Instruction::CastOps::SIToFP : llvm::Instruction::CastOps::UIToFP;
            rhs_val = builder.CreateCast(cast_op, rhs_val, lhs_type);
        }
        res = builder.CreateAdd(lhs_val, rhs_val);
    } else if (rhs_type->isFloatingPointTy()) {
        if (!lhs_type->isFloatingPointTy()) {
            const auto cast_op = is_signed(m_lhs->get_type(env)) ? llvm::Instruction::CastOps::SIToFP : llvm::Instruction::CastOps::UIToFP;
            lhs_val = builder.CreateCast(cast_op, lhs_val, rhs_type);
        }
        res = builder.CreateAdd(lhs_val, rhs_val);
    } else if (lhs_type->isIntegerTy()) {
        if (!rhs_type->isIntegerTy()) {
            return std::unexpected{llvm_error{"rhs of integral add must be integral", *this}};
        }
        auto lhs_i_type_size = llvm::cast<llvm::IntegerType>(lhs_type)->getBitWidth();
        auto rhs_i_type_size = llvm::cast<llvm::IntegerType>(rhs_type)->getBitWidth();
        if (lhs_i_type_size > rhs_i_type_size) {
            lhs_val = builder.CreateZExt(lhs_val, rhs_type);
        } else if (rhs_i_type_size > lhs_i_type_size) {
            rhs_val = builder.CreateZExt(rhs_val, lhs_type);
        }
        res = builder.CreateAdd(lhs_val, rhs_val);
    } else if (rhs_type->isIntegerTy()) {
        if (!lhs_type->isIntegerTy()) {
            return std::unexpected{llvm_error{"lhs of integral add must be integral", *this}};
        }
        auto lhs_i_type_size = llvm::cast<llvm::IntegerType>(lhs_type)->getBitWidth();
        auto rhs_i_type_size = llvm::cast<llvm::IntegerType>(rhs_type)->getBitWidth();
        if (lhs_i_type_size > rhs_i_type_size) {
            lhs_val = builder.CreateZExt(lhs_val, rhs_type);
        } else if (rhs_i_type_size > lhs_i_type_size) {
            rhs_val = builder.CreateZExt(rhs_val, lhs_type);
        }
        res = builder.CreateAdd(lhs_val, rhs_val);
    } else {
        return std::unexpected{llvm_error{"unsupported addition between types", *this}};
    }
    if (!res) {
        return std::unexpected{llvm_error{"res was nullptr", *this}};
    }
    return res;
}


}