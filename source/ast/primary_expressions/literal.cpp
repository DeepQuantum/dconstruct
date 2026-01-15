#include "ast/primary_expressions/literal.h"

#include "llvm/IR/Constant.h"

namespace dconstruct::ast {

void literal::pseudo_c(std::ostream& os) const {
    const std::string orig = primitive_to_string(m_value);
    if (os.iword(get_flag_index()) & static_cast<i32>(LANGUAGE_FLAGS::IDENTIFIER_PASCAL)) {
        const auto str_res = try_convert_pascal_case(orig);
        os << str_res.value_or(orig);
    } else {
        os << orig;
    }
}

void literal::pseudo_py(std::ostream& os) const {
    os << primitive_to_string(m_value);
}

void literal::pseudo_racket(std::ostream& os) const {
    os << primitive_to_string(m_value);
}


[[nodiscard]] expr_uptr literal::simplify() const {
    return std::make_unique<literal>(m_value);
}

[[nodiscard]] bool literal::equals(const expression& rhs) const noexcept {
    const literal* rhs_ptr = dynamic_cast<const literal*>(&rhs);
    if (rhs_ptr != nullptr) {
        return m_value == rhs_ptr->m_value;
    }
    return false;
}

[[nodiscard]] expr_uptr literal::clone() const {
    auto expr = std::make_unique<literal>(m_value);
    if (!is_unknown(m_type)) expr->set_type(m_type);
    return expr;
}

[[nodiscard]] full_type literal::compute_type(const type_environment& env) const noexcept {
    return primitive_type { kind_from_primitive_value(m_value) };
}

[[nodiscard]] u16 literal::calc_complexity() const noexcept {
    return std::visit([&](auto&& lit) -> u16 {
        using T = std::decay_t<decltype(lit)>;
        if constexpr (std::is_arithmetic_v<T>) {
            return static_cast<u16>(lit != 0);
        } else {
            return 1;
        }
    }, m_value);
}

[[nodiscard]] const literal* literal::as_literal() const noexcept {
    return this;
}

VAR_OPTIMIZATION_ACTION literal::var_optimization_pass(var_optimization_env& env)  noexcept {
    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION literal::foreach_optimization_pass(foreach_optimization_env& env)  noexcept {
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

MATCH_OPTIMIZATION_ACTION literal::match_optimization_pass(match_optimization_env& env) noexcept {
    return MATCH_OPTIMIZATION_ACTION::LITERAL;
}

[[nodiscard]] std::optional<semantic_check_error> literal::check_semantics(type_environment& env) const noexcept {
    return std::nullopt;
}


[[nodiscard]] llvm_res literal::emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>&, llvm::Module& module, const type_environment& env) const noexcept {
   return std::visit([&](auto&& lit) -> llvm_res {
        using T = std::decay_t<decltype(lit)>;
        if constexpr (std::is_floating_point_v<T>) {
            return llvm::ConstantFP::get(ctx, llvm::APFloat(lit));
        } else if constexpr (std::is_integral_v<T>) {
            return llvm::ConstantInt::get(ctx, llvm::APInt(sizeof(T) * 8, lit, std::is_signed_v<T>));
        } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
            return llvm::ConstantPointerNull::get(llvm::PointerType::get(ctx, 0));
        } else if constexpr (std::is_same_v<T, sid32_literal> || std::is_same_v<T, sid64_literal>) {
            return llvm::ConstantInt::get(ctx, llvm::APInt(sizeof(T::first) * 8, lit.first, std::is_signed_v<T>));
        } else if constexpr (std::is_same_v<T, std::string>) {
            auto* gv = new llvm::GlobalVariable(
                module,
                llvm::ArrayType::get(llvm::Type::getInt8Ty(ctx), lit.length() + 1),
                true,
                llvm::GlobalValue::PrivateLinkage,
                llvm::ConstantDataArray::getString(ctx, lit, true),
                ".str_" + std::to_string(m_emittedStringCount++)
            );
            if (!gv) {
                return std::unexpected{llvm_error{"couldn't create global variable", *this}};
            }
            return llvm::ConstantExpr::getGetElementPtr(gv->getValueType(), gv, llvm::ArrayRef<llvm::Constant*>{
                {llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0),
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0)}}
            );
        } else if constexpr (std::is_same_v<T, std::monostate>) {
            return std::unexpected{llvm_error{"tried to emit literal with unknown type", *this}};
        }
   }, m_value);
}


}