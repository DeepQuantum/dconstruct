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

[[nodiscard]] full_type call_expr::compute_type(const compiler::environment& env) const {
    return std::monostate();
};

[[nodiscard]] u16 call_expr::complexity() const noexcept {
    u16 res = 1;
    for (const auto& arg : m_arguments) {
        res += arg->complexity();
    }
    return res;
}


}