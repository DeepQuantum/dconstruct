#include "ast/unary_expressions/negate_expression.h"
#include "ast/primary_expressions/literal.h"

namespace dconstruct::ast {

// [[nodiscard]] bool negate_expr::equals(const expression& rhs) const noexcept {
//     const negate_expr* rhs_ptr = dynamic_cast<const negate_expr*>(&rhs);
//     if (rhs_ptr == nullptr) {
//         return false;
//     }
//     return m_rhs == rhs_ptr->m_rhs;
// }

[[nodiscard]] expr_uptr negate_expr::simplify() const {
    expr_uptr rhs = m_rhs->simplify();
    const literal* rhs_ptr = dynamic_cast<const literal*>(rhs.get());
    if (rhs_ptr != nullptr) {
        const primitive_value prim = rhs_ptr->m_value;
        const std::optional<primitive_number> option = get_number(prim);
        if (option.has_value()) {
            return std::make_unique<literal>(std::visit([](auto&& arg) -> primitive_value {
                return -static_cast<std::decay_t<decltype(arg)>>(arg);
            }, option.value()));
        }
        return nullptr;
    }
    return std::make_unique<negate_expr>(m_operator, std::move(rhs));
}

[[nodiscard]] expected_value_ptr negate_expr::emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) const noexcept {
    auto rhs = m_rhs->emit_llvm(ctx, builder, module);
    if (!rhs) {
        return rhs;
    }
    llvm::Value* res = builder.CreateNeg(*rhs); 
    if (!res) {
        return std::unexpected{llvm_error{"negate was nullptr", *this}};
    }
    return res;
}


}

