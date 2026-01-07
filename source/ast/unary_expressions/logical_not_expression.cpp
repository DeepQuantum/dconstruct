#include "ast/unary_expressions/logical_not_expression.h"
#include "ast/primary_expressions/literal.h"

namespace dconstruct::ast {
void logical_not_expr::pseudo_c(std::ostream &os) const {
    os << '!' << *m_rhs;
}

void logical_not_expr::pseudo_py(std::ostream &os) const {
    os << "not " << *m_rhs; 
}

void logical_not_expr::pseudo_racket(std::ostream &os) const {
    os << "(not " << *m_rhs << ')';
}

// [[nodiscard]] bool logical_not_expr::equals(const expression &rhs) const noexcept {
//     const logical_not_expr* rhs_ptr = dynamic_cast<const logical_not_expr*>(&rhs);
//     if (rhs_ptr == nullptr) {
//         return false;
//     }
//     return m_rhs == rhs_ptr->m_rhs;
// }

[[nodiscard]] expr_uptr logical_not_expr::simplify() const {
    expr_uptr rhs = m_rhs->simplify();
    const literal* rhs_ptr = dynamic_cast<const literal*>(rhs.get());
    if (rhs_ptr != nullptr) {
        const primitive_value value = rhs_ptr->m_value;
        switch(rhs_ptr->get_type()) {
            case primitive_kind::SID: 
            case primitive_kind::BOOL:    return std::make_unique<literal>(!std::get<bool>(value));
            case primitive_kind::STRING:  return std::make_unique<literal>(!(std::get<std::string>(value).empty()));
            default: {
                const std::optional<primitive_number> num_opt = get_number(value);
                if (num_opt.has_value()) {
                    return std::make_unique<literal>(std::visit([](auto&& arg) -> bool {
                        return arg == 0;
                    }, num_opt.value()));
                } 
                return nullptr;
            }
        }
    }
    return std::make_unique<logical_not_expr>(m_operator, std::move(rhs));
}

[[nodiscard]] llvm_res logical_not_expr::emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module, const type_environment& env) const noexcept {
    llvm_res rhs = m_rhs->emit_llvm(ctx, builder, module, env);
    if (!rhs) {
        return rhs;
    }
    llvm::Value* res = builder.CreateNot(*rhs);
    if (!res) {
        return std::unexpected{llvm_error{"create not was nullptr", *this}};
    }
    return res;
}

}