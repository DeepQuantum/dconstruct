#include "ast/primary_expressions/call.h"
#include "ast/primary_expressions/identifier.h"
#include "ast/primary_expressions/sid_identifier.h"
#include "ast/primary_expressions/struct_access.h"
#include "ast//primary_expressions/literal.h"

#include <algorithm>
#include <array>
#include <format>
#include <string_view>

namespace dconstruct::ast {

    namespace {
        using namespace std::literals;

        static constexpr std::array BOXED_VALUE_IDS = {
            "invalid"sv,
            "bool"sv,
            "i32"sv,
            "u32"sv,
            "i64"sv,
            "u64"sv,
            "f32"sv,
            "symbol"sv,
            "timeframePOD"sv,
            "unused"sv,
            "netTrackerId"sv,
            "processHandleMutable"sv,
            "processHandle"sv,
            "p64"sv,
            "point"sv,
            "vector"sv,
            "quat"sv,
            "locator"sv,
            "boundframe"sv,
            "stringcopy"sv,
            "unused"sv,
            "array"sv,
            "color"sv,
            "ssAnimateParams"sv,
            "NDattackInfo"sv,
        };
    }

    void call_expr::pseudo_c(ast_serialization_buffer& buffer) const {
        const bool func_name_as_pascal = buffer.has_flag(LANGUAGE_FLAGS::FUNCTION_NAMES_PASCAL);
        if (func_name_as_pascal) {
            buffer.set_flag(LANGUAGE_FLAGS::IDENTIFIER_PASCAL);
        }
        buffer.append(*m_callee, '(');
        if (func_name_as_pascal) {
            buffer.clear_flag(LANGUAGE_FLAGS::IDENTIFIER_PASCAL);
        }
        if (m_arguments.size() > 8 || get_complexity() > MAX_NON_SPLIT_COMPLEXITY && m_arguments.size() > 3) {
            buffer.append('\n');
            buffer.indent_more();
            for (u16 i = 0; i < m_arguments.size(); ++i) {
                buffer.append_indent();
                buffer.append(*m_arguments[i]);
                if (i != m_arguments.size() - 1) {
                    buffer.append(",\n"sv);
                }
            }
            buffer.indent_less();
            buffer.append('\n');
            buffer.append_indent();
            buffer.append(')');
        } else {
            for (u16 i = 0; i < m_arguments.size(); ++i) {
                buffer.append(*m_arguments[i]);
                if (i != m_arguments.size() - 1) {
                    buffer.append(", "sv);
                }
            }
            buffer.append(')');
        }
    }

    void call_expr::to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept {
        if (const std::string* name = m_callee->get_name()) {
            buffer.append(AST_COLOR::CALL, *name);
        } else {
            buffer.append(*m_callee);
        }
        buffer.append(AST_COLOR::PUNCTUATION, '(');
        if (m_arguments.size() > 8 || (get_complexity() > MAX_NON_SPLIT_COMPLEXITY && m_arguments.size() > 3)) {
            buffer.append(AST_COLOR::BLANK, '\n');
            buffer.indent_more();
            for (u16 i = 0; i < m_arguments.size(); ++i) {
                buffer.append_indent();
                buffer.append(*m_arguments[i]);
                if (i != m_arguments.size() - 1) {
                    buffer.append(AST_COLOR::PUNCTUATION, ",\n"sv);
                }
            }
            buffer.indent_less();
            buffer.append(AST_COLOR::BLANK, '\n');
            buffer.append_indent();
            buffer.append(AST_COLOR::PUNCTUATION, ')');
        } else {
            for (u16 i = 0; i < m_arguments.size(); ++i) {
                buffer.append(*m_arguments[i]);
                if (i != m_arguments.size() - 1) {
                    buffer.append(AST_COLOR::PUNCTUATION, ", "sv);
                }
            }
            buffer.append(AST_COLOR::PUNCTUATION, ')');
        }
    }

    void call_expr::pseudo_c_for_compiler(ast_serialization_buffer& buffer) const {
        buffer.set_flag(LANGUAGE_FLAGS::COMPILER);

        std::string callee_name;
        if (const auto* sid = dynamic_cast<const sid_identifier*>(m_callee.get())) {
            callee_name = sid->m_name.m_lexeme;
        } else if (const auto* id = dynamic_cast<const identifier*>(m_callee.get())) {
            callee_name = id->m_name.m_lexeme;
        }

        if ((callee_name == "#dc:format" || callee_name == "dc:format") && m_arguments.size() >= 2) {
            buffer.append(*m_arguments[0], " $ "sv);
            for (u16 i = 1; i < m_arguments.size(); ++i) {
                buffer.append(*m_arguments[i]);
                if (i + 1 != m_arguments.size()) {
                    buffer.append(", "sv);
                }
            }
            return;
        }

        if ((callee_name == "#display" || callee_name == "display") && !m_arguments.empty()) {
            buffer.append(">> "sv, *m_arguments[0]);
            return;
        }

        if ((callee_name == "#go" || callee_name == "go") && !m_arguments.empty()) {
            buffer.append("=> "sv, *m_arguments[0]);
            return;
        }

        buffer.append(*m_callee, '(');
        for (u16 i = 0; i < m_arguments.size(); ++i) {
            buffer.append(*m_arguments[i]);
            if (i + 1 != m_arguments.size()) {
                buffer.append(", "sv);
            }
        }
        buffer.append(')');
    }

    void call_expr::pseudo_py(ast_serialization_buffer& buffer) const {
        const bool func_name_as_pascal = buffer.has_flag(LANGUAGE_FLAGS::FUNCTION_NAMES_PASCAL);
        if (func_name_as_pascal) {
            buffer.set_flag(LANGUAGE_FLAGS::IDENTIFIER_PASCAL);
        }
        buffer.append(*m_callee, '(');
        if (func_name_as_pascal) {
            buffer.clear_flag(LANGUAGE_FLAGS::IDENTIFIER_PASCAL);
        }
        for (u16 i = 0; i < m_arguments.size(); ++i) {
            buffer.append(*m_arguments[i]);
            if (i != m_arguments.size() - 1) {
                buffer.append(", "sv);
            }
        }
        buffer.append(')');
    }

    void call_expr::pseudo_racket(ast_serialization_buffer& buffer) const {
        buffer.append('(', *m_callee);
        for (const auto& arg : m_arguments) {
            buffer.append(' ', *arg);
        }
        buffer.append(')');
    }

    [[nodiscard]] expr_uptr call_expr::simplify() const {
        std::vector<expr_uptr> args{};
        for (const auto& arg : m_arguments) {
            args.push_back(arg->simplify());
        }
        return std::make_unique<call_expr>(m_token, m_callee->clone(), std::move(args));
    }

    [[nodiscard]] expr_uptr call_expr::clone() const {
        std::vector<expr_uptr> args{};
        for (const auto& arg : m_arguments) {
            args.push_back(arg->clone());
        }
        auto expr = std::make_unique<call_expr>(m_token, m_callee->clone(), std::move(args));
        if (m_type)
            expr->set_type(*m_type);
        return expr;
    }

    [[nodiscard]] bool call_expr::equals(const expression& rhs) const noexcept {
        const call_expr* rhs_ptr = dynamic_cast<const call_expr*>(&rhs);
        if (rhs_ptr == nullptr) {
            return false;
        }
        return m_callee == rhs_ptr->m_callee && m_arguments == rhs_ptr->m_arguments;
    }

    [[nodiscard]] u16 call_expr::calc_complexity() const noexcept {
        u16 res = 1;
        for (const auto& arg : m_arguments) {
            res += arg->get_complexity();
        }
        return res;
    }

    [[nodiscard]] expr_uptr* call_expr::get_first_argument() noexcept {
        assert(m_arguments.size() > 0);
        return &m_arguments[0];
    }

    [[nodiscard]] full_type call_expr::compute_type_unchecked(const compilation::scope& env) const noexcept {
        const full_type callee_type = m_callee->get_type_unchecked(env);
        assert(std::holds_alternative<function_type>(callee_type));
        return *std::get<function_type>(callee_type).m_return;
    }

    [[nodiscard]] semantic_check_res call_expr::compute_type_checked(compilation::scope& env) const noexcept {
        const semantic_check_res callee_type = m_callee->get_type_checked(env);
        if (!callee_type) {
            return callee_type;
        }
        if (!std::holds_alternative<function_type>(*callee_type)) {
            return std::unexpected{semantic_check_error{"expected callable type but got " + type_to_declaration_string(*callee_type), this}};
        }
        const function_type func_type = std::get<function_type>(*callee_type);

        if (!func_type.m_isVariadic && func_type.m_arguments.size() != m_arguments.size()) {
            return std::unexpected{semantic_check_error{"expected " + std::to_string(func_type.m_arguments.size()) + " arguments but got " + std::to_string(m_arguments.size()), this}};
        }
        if (func_type.m_isVariadic && m_arguments.size() < func_type.m_arguments.size()) {
            return std::unexpected{semantic_check_error{"expected at least " + std::to_string(func_type.m_arguments.size()) + " arguments but got " + std::to_string(m_arguments.size()), this}};
        }

        const u64 arg_counter = func_type.m_isVariadic ? m_arguments.size() : func_type.m_arguments.size();

        for (u32 i = 0; i < arg_counter; ++i) {
            const expr_uptr& arg = m_arguments[i];
            const semantic_check_res arg_type = arg->get_type_checked(env);
            if (!arg_type) {
                return arg_type;
            }
            if (i < func_type.m_arguments.size() && not_assignable_reason(*func_type.m_arguments[i].second, *arg_type)) {
                return std::unexpected{semantic_check_error{
                    "expected argument of type " + type_to_declaration_string(*func_type.m_arguments[i].second) + " at position " + std::to_string(i) + " but got " + type_to_declaration_string(*arg_type), arg.get()}};
            }
        }

        return *func_type.m_return;
    }

    [[nodiscard]] emission_res call_expr::emit_dc(
        compilation::function_context& fn,
        compilation::global_state& global,
        const std::optional<reg_idx> destination
    ) const noexcept {
        fn.push_deferred();

        emission_res callee;
        if (destination) {
            const emission_res callee_destination = (*destination >= ARGUMENT_REGISTERS_IDX) ? fn.get_next_unused_register() : *destination;
            if (!callee_destination) {
                return callee_destination;
            }
            callee = m_callee->emit_dc_callee(fn, global, *callee_destination);
        } else {
            callee = m_callee->emit_dc_callee(fn, global);
        }

        if (!callee) {
            return callee;
        }

        for (u32 i = 0; i < m_arguments.size(); ++i) {
            const emission_res arg_reg = m_arguments[i]->emit_dc(fn, global, ARGUMENT_REGISTERS_IDX + i);
            if (!arg_reg) {
                return arg_reg;
            }
            if (*arg_reg != ARGUMENT_REGISTERS_IDX + i) {
                fn.emit_instruction(Opcode::Move, ARGUMENT_REGISTERS_IDX + i, *arg_reg);
            }
        }

        const std::optional<full_type> callee_type = m_callee->get_type();
        assert(callee_type);
        assert(std::holds_alternative<function_type>(*callee_type));
        const function_type& ftype = std::get<function_type>(*callee_type);

        const Opcode call_opcode = ftype.m_distanceType == function_type::DISTANCE::FAR ? Opcode::CallFf : Opcode::Call;

        fn.emit_instruction(call_opcode, *callee, *callee, m_arguments.size());

        fn.pop_deferred();

        return *callee;
    }

    VAR_OPTIMIZATION_ACTION call_expr::var_optimization_pass(var_optimization_env& env) noexcept {
        for (auto& arg : m_arguments) {
            env.check_action(&arg);
        }
        return VAR_OPTIMIZATION_ACTION::NONE;
    }

    FOREACH_OPTIMIZATION_ACTION call_expr::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
        if (const std::string* name = m_callee->get_name()) {
            const sid64 id = SID(name->c_str());
            switch (id) {
                case SID("begin-foreach") :
                    {
                    return FOREACH_OPTIMIZATION_ACTION::BEGIN_FOREACH;
                }
                case SID("end-foreach") :
                    {
                    return FOREACH_OPTIMIZATION_ACTION::END_FOREACH;
                }
                case SID("ddict-key-at"):
                case SID("ddict-value-at"):
                case SID("darray-at") :
                    {
                    return FOREACH_OPTIMIZATION_ACTION::ITERABLE_AT;
                }
                case SID("ddict-key-count"):
                case SID("darray-count") :
                    {
                    return FOREACH_OPTIMIZATION_ACTION::ITERABLE_COUNT;
                }
            }
        }
        for (auto& arg : m_arguments) {
            env.check_action(&arg);
        }
        return FOREACH_OPTIMIZATION_ACTION::NONE;
    }

    MATCH_OPTIMIZATION_ACTION call_expr::match_optimization_pass(match_optimization_env& env) noexcept {
        return MATCH_OPTIMIZATION_ACTION::NONE;
    }


    [[nodiscard]] std::unique_ptr<struct_access> call_expr::to_struct_access() noexcept {
        replace_if_struct_access(m_callee);
        for (auto& argument : m_arguments) {
            replace_if_struct_access(argument);
        }
        return nullptr;
    }

    void call_expr::regex_optimization_pass() noexcept {
        if (m_callee->to_pseudo_c_string() == "new-boxed-value") {
            const literal* boxed_kind_literal = m_arguments[0]->as_literal();
            const std::optional boxed_kind_num_res = get_raw_number(boxed_kind_literal->m_value);
            assert(boxed_kind_num_res);

            std::string new_name = std::format("boxed_{}", BOXED_VALUE_IDS[*boxed_kind_num_res]);
            
            m_callee = std::make_unique<identifier>(std::move(new_name));

            m_arguments.erase(m_arguments.begin());
        }

        m_callee->regex_optimization_pass();
        std::for_each(m_arguments.begin(), m_arguments.end(), [](expr_uptr& arg) {
            arg->regex_optimization_pass();
        });
    }

    // [[nodiscard]] llvm_res call_expr::emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module, const compilation::scope& env) const noexcept {
    //     const identifier* callee_id = dynamic_cast<identifier*>(m_callee.get());
    //     if (!callee_id) {
    //         return std::unexpected{llvm_error{"expected identifier for callee but got non-identifier (not implemented)", *this}};
    //     }
    //
    //     llvm::Function* callee_f = module.getFunction(callee_id->m_name.m_lexeme);
    //
    //     if (callee_f->arg_size() != m_arguments.size()) {
    //         std::string error = "llvm function expects " + std::to_string(callee_f->arg_size()) + " arguments but ast call contains " + std::to_string(m_arguments.size()) + " arguments";
    //         return std::unexpected{llvm_error{std::move(error), *this}};
    //     }
    //
    //     std::vector<llvm::Value*> args_v;
    //     args_v.reserve(m_arguments.size());
    //
    //     for (const auto& arg : m_arguments) {
    //         auto exp_value = arg->emit_llvm(ctx, builder, module, env);
    //         if (!exp_value) {
    //             return exp_value;
    //         }
    //         args_v.push_back(*exp_value);
    //     }
    //
    //
    //     auto res = builder.CreateCall(callee_f, args_v);
    //     if (!res) {
    //         return std::unexpected{llvm_error{"expected non-null result from function call but got nullptr", *this}};
    //     }
    //     return res;
    // }

}
