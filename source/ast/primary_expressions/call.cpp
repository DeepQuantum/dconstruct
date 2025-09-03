#include "ast/primary_expressions/call.h"


namespace dconstruct::ast {

void call_expr::pseudo(std::ostream& os) const {
    os << *m_callee << '(';
    for (u16 i = 0; i < m_arguments.size(); ++i) {
        os << *m_arguments[i];
        if (i != m_arguments.size() - 1) {
            os << ", ";
        }
    }
    os << ')';
}

void call_expr::ast(std::ostream& os) const {
    os << "call[callee=" << *m_callee << ";args={";
    for (u16 i = 0; i < m_arguments.size(); ++i) {
        os << *m_arguments[i];
        if (i != m_arguments.size() - 1) {
            os << ",";
        }
    }
    os << "}]";
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
    return std::make_unique<call_expr>(m_token, m_callee->clone(), std::move(args));
}


[[nodiscard]] b8 call_expr::equals(const expression &rhs) const noexcept {
    const call_expr* rhs_ptr = dynamic_cast<const call_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_callee == rhs_ptr->m_callee && m_arguments == rhs_ptr->m_arguments;
}

[[nodiscard]] std::optional<full_type> call_expr::compute_type(const compiler::environment& env) const {
    return std::nullopt;
};


}