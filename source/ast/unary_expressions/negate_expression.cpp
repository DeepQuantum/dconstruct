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

[[nodiscard]] full_type negate_expr::compute_type_unchecked(const compilation::scope& env) const noexcept {
    return m_rhs->get_type_unchecked(env);
}

[[nodiscard]] semantic_check_res negate_expr::compute_type_checked(compilation::scope& env) const noexcept {
    const semantic_check_res rhs_res = m_rhs->get_type_checked(env);

    if (!rhs_res) {
        return rhs_res;
    }

    const std::optional<std::string> invalid_negate = std::visit([](auto&& rhs_type) -> std::optional<std::string> {
        using T = std::decay_t<decltype(rhs_type)>;

        if constexpr (is_primitive<T>) {
            if (is_arithmetic(rhs_type.m_type)) {
                return std::nullopt;
            } else {
                return "cannot negate expression with non-integral type " + type_to_declaration_string(rhs_type);
            }
        } else {
            return "cannot negate expression with non-integral type " + type_to_declaration_string(rhs_type);
        }
    }, *rhs_res);

    if (!invalid_negate) {
        return make_type_from_prim(primitive_kind::U64);
    }
    
    return std::unexpected{semantic_check_error{*invalid_negate, this}};
}


[[nodiscard]] emission_res negate_expr::emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> opt_destination) const noexcept {
    if (const literal* literal_rhs = m_rhs->as_literal(); is_arithmetic(literal_rhs->get_type())) {
        const std::optional<primitive_number> value = get_number(literal_rhs->m_value);
        assert(value);
        const primitive_number new_value = std::visit([](auto&& arg) -> primitive_number {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, u8> || std::is_same_v<T, u16> || std::is_same_v<T, u32>) {
                return -static_cast<i32>(arg);
            } else if constexpr (std::is_same_v<T, u64>) {
                return -static_cast<i64>(arg);
            } else {
                return -arg;
            }
        }, *value);
        //const table_idx = fn.add_to_symbol_table(std::bit_cast<u64>(new_value), )

    } else {
        const emission_res rhs_res = m_rhs->emit_dc(fn, global);

        if (!rhs_res) {
            return rhs_res;
        }

        assert(std::holds_alternative<primitive_type>(*m_type));
        const Opcode opcode = is_integral(std::get<primitive_type>(*m_type).m_type) ? Opcode::INeg : Opcode::FNeg;

        const emission_res neg_destination = fn.get_destination(opt_destination);
        if (!neg_destination) {
            return neg_destination;
        }

        fn.emit_instruction(opcode, *neg_destination, *rhs_res);
        fn.free_register(*rhs_res);
        return *neg_destination;
    }    
}


[[nodiscard]] llvm_res negate_expr::emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module, const compilation::scope& env) const noexcept {
    auto rhs = m_rhs->emit_llvm(ctx, builder, module, env);
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

