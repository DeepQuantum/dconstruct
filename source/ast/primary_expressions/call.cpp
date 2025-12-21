#include "ast/primary_expressions/call.h"
#include "ast/primary_expressions/identifier.h"


namespace dconstruct::ast {

void call_expr::pseudo_c(std::ostream& os) const {
    os << *m_callee << '(';
    for (u16 i = 0; i < m_arguments.size(); ++i) {
        os << *m_arguments[i];
        if (i != m_arguments.size() - 1) {
            os << ", ";
        }
    }
    os << ')';
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
    if (!is_unknown(m_type)) expr->set_type(m_type);
    return expr;
}


[[nodiscard]] bool call_expr::equals(const expression &rhs) const noexcept {
    const call_expr* rhs_ptr = dynamic_cast<const call_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_callee == rhs_ptr->m_callee && m_arguments == rhs_ptr->m_arguments;
}

[[nodiscard]] full_type call_expr::compute_type(const type_environment& env) const {
    return std::monostate();
};

[[nodiscard]] u16 call_expr::complexity() const noexcept {
    u16 res = 1;
    for (const auto& arg : m_arguments) {
        res += arg->complexity();
    }
    return res;
}

bool call_expr::decomp_optimization_pass(second_pass_env& env) noexcept {
    for (auto& arg : m_arguments) {
        if (arg->decomp_optimization_pass(env)) {
            env->lookup(static_cast<identifier&>(*arg).m_name.m_lexeme)->m_firstUsageSite = &arg;
        }
    }
    return false;
}

[[nodiscard]] expec_llvm_value call_expr::emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module, const type_environment& env) const noexcept {
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