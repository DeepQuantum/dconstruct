#include "ast/unary_expressions/bitwise_not_expression.h"

namespace dconstruct::ast {

void bitwise_not_expr::pseudo_racket(std::ostream& os) const {
    os << "(bitwise-not " << *m_rhs << ")";
}

[[nodiscard]] expr_uptr bitwise_not_expr::simplify() const {
    return nullptr;
}

[[nodiscard]] semantic_check_res bitwise_not_expr::compute_type_checked(compilation::scope& env) const noexcept {
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
            return "expected integral type for bitwise not but got " + type_to_declaration_string(arg);
        } else {
            return "expected integral type for bitwise not but got " + type_to_declaration_string(arg);
        }
    }, *rhs_type);

    if (!invalid_bitwise_not) {
        return make_type_from_prim(primitive_kind::U64);
    }
    return std::unexpected{semantic_check_error{*invalid_bitwise_not, this}};
}

[[nodiscard]] emission_res bitwise_not_expr::emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination) const noexcept {
    const emission_res rhs_res = m_rhs->emit_dc(fn, global);
    if (!rhs_res) {
        return rhs_res;
    }

    const emission_res not_destination = fn.get_destination(destination);
    if (!not_destination) {
        return not_destination;
    }

    fn.emit_instruction(Opcode::OpBitNot, *not_destination, *rhs_res);
    fn.free_register(*rhs_res);
    return *not_destination;
}


[[nodiscard]] llvm_res bitwise_not_expr::emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module, const compilation::scope& env) const noexcept {
    auto rhs = m_rhs->emit_llvm(ctx, builder, module, env);
    if (!rhs) {
        return rhs;
    }
    llvm::Value* res = builder.CreateNeg(*rhs);
    if (!res) {
        return std::unexpected{llvm_error{"expected non-null result from bitwise not but got nullptr", *this}};
    }
    return res;
}


}