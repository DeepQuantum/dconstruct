#include "ast/unary_expressions/bitwise_not_expression.h"

namespace dconstruct::ast {

[[nodiscard]] expr_uptr bitwise_not_expr::simplify() const {
    return nullptr;
}

[[nodiscard]] semantic_check_res bitwise_not_expr::compute_type_checked(type_environment& env) const noexcept {
    const semantic_check_res rhs_type = m_rhs->get_type_checked(env);
    if (!rhs_type) {
        return rhs_type;
    }

    const std::optional<std::string> invalid_bitwise_not = std::visit([](auto&& arg) -> std::optional<std::string> {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (is_primitive<T>) {
            if (is_integral(arg.m_type)) {
                return std::nullopt;
            }
            return "cannot negate expression with non-integral type " + type_to_declaration_string(arg);
        } else {
            return "cannot negate expression with non-integral type " + type_to_declaration_string(arg);
        }
    }, *rhs_type);

    if (!invalid_bitwise_not) {
        return make_type_from_prim(primitive_kind::U64);
    }
    return std::unexpected{semantic_check_error{*invalid_bitwise_not, this}};
}


[[nodiscard]] llvm_res bitwise_not_expr::emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module, const type_environment& env) const noexcept {
    auto rhs = m_rhs->emit_llvm(ctx, builder, module, env);
    if (!rhs) {
        return rhs;
    }
    llvm::Value* res = builder.CreateNeg(*rhs);
    if (!res) {
        return std::unexpected{llvm_error{"xor was nullptr", *this}};
    }
    return res;
}


}