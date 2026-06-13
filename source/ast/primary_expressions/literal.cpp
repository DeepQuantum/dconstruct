#include "ast/primary_expressions/literal.h"
#include "ast/primary_expressions/struct_access.h"

// #include "llvm/IR/Constant.h"

namespace dconstruct::ast {

    void literal::pseudo_c(ast_serialization_buffer& buffer) const {
        const std::string orig = primitive_to_string(m_value);
        if (buffer.has_flag(LANGUAGE_FLAGS::IDENTIFIER_PASCAL)) {
            const auto str_res = try_convert_pascal_case(orig);
            buffer.append(str_res.value_or(orig));
        } else {
            buffer.append(orig);
        }
    }

    void literal::pseudo_py(ast_serialization_buffer& buffer) const {
        const std::string orig = primitive_to_string(m_value);
        if (buffer.has_flag(LANGUAGE_FLAGS::IDENTIFIER_PASCAL)) {
            const auto str_res = try_convert_pascal_case(orig);
            buffer.append(str_res.value_or(orig));
        } else {
            buffer.append(orig);
        }
    }

    void literal::pseudo_racket(ast_serialization_buffer& buffer) const {
        const std::string orig = primitive_to_string(m_value);
        if (buffer.has_flag(LANGUAGE_FLAGS::IDENTIFIER_PASCAL)) {
            const auto str_res = try_convert_pascal_case(orig);
            buffer.append(str_res.value_or(orig));
        } else {
            buffer.append(orig);
        }
    }

    void literal::to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept {
        const AST_COLOR color = std::visit([](auto&& value) -> AST_COLOR {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, char>) {
                return AST_COLOR::STRING;
            } else if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, std::nullptr_t> || std::is_same_v<T, std::monostate>) {
                return AST_COLOR::KEYWORD;
            } else {
                return AST_COLOR::NUMBER;
            }
        }, m_value);
        buffer.append(color, primitive_to_string(m_value));
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
        if (m_type)
            expr->set_type(*m_type);
        return expr;
    }

    [[nodiscard]] full_type literal::compute_type_unchecked(const compilation::scope& env) const noexcept {
        return primitive_type{kind_from_primitive_value(m_value)};
    }

    [[nodiscard]] u16 literal::calc_complexity() const noexcept {
        return std::visit(
            [&](auto&& lit) -> u16 {
                using T = std::decay_t<decltype(lit)>;
                if constexpr (std::is_arithmetic_v<T>) {
                    return static_cast<u16>(lit != 0);
                } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    return 0;
                } else {
                    return 1;
                }
            },
            m_value
        );
    }

    [[nodiscard]] const literal* literal::as_literal() const noexcept {
        return this;
    }

    VAR_OPTIMIZATION_ACTION literal::var_optimization_pass(var_optimization_env& env) noexcept {
        return VAR_OPTIMIZATION_ACTION::NONE;
    }

    FOREACH_OPTIMIZATION_ACTION literal::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
        return FOREACH_OPTIMIZATION_ACTION::NONE;
    }

    MATCH_OPTIMIZATION_ACTION literal::match_optimization_pass(match_optimization_env& env) noexcept {
        return MATCH_OPTIMIZATION_ACTION::LITERAL;
    }

    void literal::regex_optimization_pass() noexcept {}

    [[nodiscard]] std::unique_ptr<struct_access> literal::to_struct_access() noexcept {
        return nullptr;
    }

    [[nodiscard]] semantic_check_res literal::compute_type_checked(compilation::scope& env) const noexcept {
        const primitive_kind type = kind_from_primitive_value(m_value);
        if (type == primitive_kind::NOTHING) {
            return std::unexpected{semantic_check_error{"literal has unknown type", this}};
        }
        return primitive_type{type};
    }

    [[nodiscard]] std::optional<i64> literal::raw_pattern_number() const noexcept {
        return get_raw_number(m_value);
    }

    [[nodiscard]] resstr<u16> literal::emit_to_symbol_table(
        compilation::function_context& fn,
        compilation::global_state& global
    ) const noexcept {
        return std::visit(
            [&](auto&& lit) -> resstr<u16> {
                using T = std::decay_t<decltype(lit)>;

                if constexpr (std::is_same_v<T, std::string>) {
                    if (m_type && std::holds_alternative<primitive_type>(*m_type)) {
                        const primitive_kind kind = std::get<primitive_type>(*m_type).m_type;
                        if (kind == primitive_kind::SID || kind == primitive_kind::SID32) {
                            return fn.add_to_symbol_table(SID(lit.c_str()));
                        }
                    }
                    const u64 size = global.add_string(std::move(lit));
                    return fn.add_to_symbol_table(size, compilation::function_context::SYMBOL_TABLE_POINTER_KIND::STRING);
                } else if constexpr (std::is_integral_v<T>) {
                    return fn.add_to_symbol_table(static_cast<u64>(lit));
                } else if constexpr (std::is_same_v<T, f32>) {
                    return fn.add_to_symbol_table(static_cast<u64>(std::bit_cast<u32>(lit)));
                } else if constexpr (std::is_same_v<T, f64>) {
                    return fn.add_to_symbol_table(std::bit_cast<u64>(lit));
                } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    return fn.add_to_symbol_table(0);
                } else {
                    return std::unexpected{"can't add " + to_pseudo_c_string() + " to symbol table"};
                }
            },
            m_value
        );
    }

    [[nodiscard]] resstr<reg_idx> literal::emit_dc(
        compilation::function_context& fn,
        compilation::global_state& global,
        const std::optional<reg_idx> destination
    ) const noexcept {
        return std::visit(
            [&](auto&& lit) -> resstr<reg_idx> {
                using T = std::decay_t<decltype(lit)>;

                resstr<reg_idx> literal_dest = fn.fix_destination(destination);
                if (!literal_dest) {
                    return literal_dest;
                }

                if constexpr (std::is_same_v<T, std::string>) {
                    if (m_type && std::holds_alternative<primitive_type>(*m_type)) {
                        const primitive_kind kind = std::get<primitive_type>(*m_type).m_type;
                        if (kind == primitive_kind::SID || kind == primitive_kind::SID32) {
                            const u8 table_idx = fn.add_to_symbol_table(SID(lit.c_str()));
                            fn.emit_instruction(Opcode::LoadStaticU64Imm, *literal_dest, table_idx);
                            return *literal_dest;
                        }
                    }
                    const u64 size = global.add_string(std::move(lit));
                    const u8 table_idx = fn.add_to_symbol_table(size, compilation::function_context::SYMBOL_TABLE_POINTER_KIND::STRING);
                    fn.emit_instruction(Opcode::LoadStaticPointerImm, *literal_dest, table_idx);
                    return *literal_dest;
                } else if constexpr (std::is_integral_v<T>) {
                    if constexpr (sizeof(T) <= 2) {
                        const u8 lo = static_cast<u16>(lit) & 0xFF;
                        const u8 hi = (static_cast<u16>(lit) >> 8) & 0xFF;
                        fn.emit_instruction(Opcode::LoadU16Imm, *literal_dest, lo, hi);
                    } else {
                        const u8 table_idx = fn.add_to_symbol_table(lit);
                        fn.emit_instruction(Opcode::LoadStaticU64Imm, *literal_dest, table_idx);
                    }
                } else if constexpr (std::is_same_v<T, f32>) {
                    if (lit == 0) {
                        fn.emit_instruction(Opcode::LoadU16Imm, *literal_dest, 0, 0);
                    } else {
                        const u64 value = std::bit_cast<u32>(lit);
                        const u8 table_idx = fn.add_to_symbol_table(value);
                        fn.emit_instruction(Opcode::LoadStaticFloatImm, *literal_dest, table_idx);
                    }
                } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    fn.emit_instruction(Opcode::LoadU16Imm, *literal_dest, 0, 0);
                } else {
                    return std::unexpected{"can't compile " + to_pseudo_c_string()};
                }

                return *literal_dest;
            },
            m_value
        );
    }

    // [[nodiscard]] llvm_res literal::emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>&, llvm::Module& module, const compilation::scope& env) const noexcept {
    //    return std::visit([&](auto&& lit) -> llvm_res {
    //         using T = std::decay_t<decltype(lit)>;
    //         if constexpr (std::is_floating_point_v<T>) {
    //             return llvm::ConstantFP::get(ctx, llvm::APFloat(lit));
    //         } else if constexpr (std::is_integral_v<T>) {
    //             return llvm::ConstantInt::get(ctx, llvm::APInt(sizeof(T) * 8, lit, std::is_signed_v<T>));
    //         } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
    //             return llvm::ConstantPointerNull::get(llvm::PointerType::get(ctx, 0));
    //         } else if constexpr (std::is_same_v<T, std::string>) {
    //             auto* gv = new llvm::GlobalVariable(
    //                 module,
    //                 llvm::ArrayType::get(llvm::Type::getInt8Ty(ctx), lit.length() + 1),
    //                 true,
    //                 llvm::GlobalValue::PrivateLinkage,
    //                 llvm::ConstantDataArray::getString(ctx, lit, true),
    //                 ".str_" + std::to_string(m_emittedStringCount++)
    //             );
    //             if (!gv) {
    //                 return std::unexpected{llvm_error{"couldn't create global variable", *this}};
    //             }
    //             return llvm::ConstantExpr::getGetElementPtr(gv->getValueType(), gv, llvm::ArrayRef<llvm::Constant*>{
    //                 {llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0),
    //                 llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0)}}
    //             );
    //         } else if constexpr (std::is_same_v<T, std::monostate>) {
    //             return std::unexpected{llvm_error{"tried to emit literal with unknown type", *this}};
    //         }
    //    }, m_value);
    // }

}
