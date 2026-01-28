#include "ast/primary_expressions/call.h"
#include "ast/primary_expressions/identifier.h"
#include "ast//primary_expressions/literal.h"

namespace dconstruct::ast {

void call_expr::pseudo_c(std::ostream& os) const {
    const bool func_name_as_pascal = os.iword(get_flag_index()) & static_cast<i32>(LANGUAGE_FLAGS::FUNCTION_NAMES_PASCAL);
    if (func_name_as_pascal) {
        os << id_pascal_case;
    }
    os << *m_callee << "(";
    if (func_name_as_pascal) {
        os << remove_id_pascal_case;
    }
    if (m_arguments.size() > 8 || get_complexity() > MAX_NON_SPLIT_COMPLEXITY && m_arguments.size() > 3) {
        os << "\n" << indent_more;
        for (u16 i = 0; i < m_arguments.size(); ++i) {
            os << indent << *m_arguments[i];
            if (i != m_arguments.size() - 1) {
                os << ",\n";
            }
        }
        os << indent_less;
        os << "\n" << indent << ")";
    } else {
        for (u16 i = 0; i < m_arguments.size(); ++i) {
            os << *m_arguments[i];
            if (i != m_arguments.size() - 1) {
                os << ", ";
            }
        }
        os << ')';
    }
}

void call_expr::pseudo_py(std::ostream& os) const {
    os << *m_callee << '(';
    for (u16 i = 0; i < m_arguments.size(); ++i) {
        os << *m_arguments[i];
        if (i != m_arguments.size() - 1) {
            os << ", ";
        }
    }
    os << ')';
}

void call_expr::pseudo_racket(std::ostream& os) const {
    const bool is_builtin = static_cast<const ast::identifier&>(*m_callee).m_name.m_lexeme == "and" || static_cast<const ast::identifier&>(*m_callee).m_name.m_lexeme == "or";
	if (!is_builtin) {
        os << '(' << *m_callee;
        for (const auto& arg : m_arguments) {
            os << " " << * arg;
        }
        os << ')';
    } else {
        os << '(' << *m_callee << "\n";
		os << indent_more;
        for (const auto& arg : m_arguments) {
            os << indent << *arg << "\n";
        }
        os << indent_less;
        os << indent << ')';
    }
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
    if (m_type) expr->set_type(*m_type);
    return expr;
}


[[nodiscard]] bool call_expr::equals(const expression &rhs) const noexcept {
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
        return std::unexpected{semantic_check_error{"callee is not of callable type: " + type_to_declaration_string(*callee_type)}};
    }
    const function_type func_type = std::get<function_type>(*callee_type);

    if (func_type.m_arguments.size() != m_arguments.size()) {
        return std::unexpected{semantic_check_error{"function expects " + std::to_string(func_type.m_arguments.size()) + " arguments but " + std::to_string(m_arguments.size()) + " were passed"}};
    }

    for (u32 i = 0; i < m_arguments.size(); ++i) {
        const expr_uptr& arg = m_arguments[i];
        const semantic_check_res arg_type = arg->get_type_checked(env);
        if (!arg_type) {
            return arg_type;
        }
        if (is_assignable(*func_type.m_arguments[i].second, *arg_type)) {
            return std::unexpected{semantic_check_error{
                "expected argument of type " + type_to_declaration_string(*func_type.m_arguments[i].second) + " at position " + std::to_string(i) + " but got " + type_to_declaration_string(*arg_type)
            , arg.get()}};
        }
    }

    return *func_type.m_return;
}

[[nodiscard]] emission_res call_expr::emit_dc(compilation::function& fn, compilation::global_state& global, const std::optional<reg_idx> destination) const noexcept {
    // if (arg_pos && *arg_pos != 0) {
    //     fn.save_used_argument_registers(std::min(static_cast<u64>(*arg_pos), m_arguments.size()));
    // }
    fn.m_deferred.push_back({});

    emission_res callee;
    if (destination) {
        const emission_res callee_destination = (*destination >= ARGUMENT_REGISTERS_IDX) ? fn.get_next_unused_register() : *destination;
        if (!callee_destination) {
            return callee_destination;
        }
        callee = m_callee->emit_dc(fn, global, *callee_destination);
    } else { 
        callee = m_callee->emit_dc(fn, global);
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

    const Opcode call_opcode = ftype.m_isFarCall ? Opcode::CallFf : Opcode::Call;

    fn.emit_instruction(call_opcode, *callee, *callee, m_arguments.size());
    fn.pop_deferred();

    // if (arg_pos && *arg_pos != 0) {
    //     fn.restore_used_argument_registers();
    // }

    return *callee;
}

VAR_OPTIMIZATION_ACTION call_expr::var_optimization_pass(var_optimization_env& env)  noexcept {
    for (auto& arg : m_arguments) {
        env.check_action(&arg);
    }
    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION call_expr::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    if (const auto* literal = m_callee->as_literal()) {
        if (!std::holds_alternative<sid64_literal>(literal->m_value)) {
            return FOREACH_OPTIMIZATION_ACTION::NONE;
        }
        const auto id = std::get<sid64_literal>(literal->m_value).first;
        switch (id) {
            case SID("begin-foreach"): {
                return FOREACH_OPTIMIZATION_ACTION::BEGIN_FOREACH;
            }
            case SID("end-foreach"): {
                return FOREACH_OPTIMIZATION_ACTION::END_FOREACH;
            }
            case SID("ddict-key-at"):
            case SID("ddict-value-at"):
            case SID("darray-at"): {
                return FOREACH_OPTIMIZATION_ACTION::ITERABLE_AT;
            }  
            case SID("ddict-key-count"): 
            case SID("darray-count"): {
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

[[nodiscard]] llvm_res call_expr::emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module, const compilation::scope& env) const noexcept {
    const ast::identifier* callee_id = dynamic_cast<ast::identifier*>(m_callee.get());
    if (!callee_id) {
        return std::unexpected{llvm_error{"callee wasn't an identifier, which is not implemented yet", *this}};
    }
    
    llvm::Function* callee_f = module.getFunction(callee_id->m_name.m_lexeme);

    if (callee_f->arg_size() != m_arguments.size()) {
        std::string error = "llvm function expects " + std::to_string(callee_f->arg_size()) + " arguments but ast call contains " + std::to_string(m_arguments.size()) + " arguments";
        return std::unexpected{llvm_error{std::move(error), *this}};
    }

    std::vector<llvm::Value*> args_v;
    args_v.reserve(m_arguments.size());

    for (const auto& arg : m_arguments) {
        auto exp_value = arg->emit_llvm(ctx, builder, module, env);
        if (!exp_value) {
            return exp_value;
        }
        args_v.push_back(*exp_value);
    }

    auto res = builder.CreateCall(callee_f, args_v);
    if (!res) {
        return std::unexpected{llvm_error{"function call was nullptr", *this}};
    }
    return res;
}


}