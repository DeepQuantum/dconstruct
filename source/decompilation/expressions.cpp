#include "expressions.h"

namespace dconstruct::dcompiler {

void add_expr::pseudo(std::ostream& os) const noexcept {
    return m_lhs->pseudo(os);
    os << " + ";
    m_rhs->pseudo(os);
}

void add_expr::ast(std::ostream& os) const noexcept {
    os << "add[";
    m_lhs->ast(os);
    os << ", ";
    m_rhs->ast(os);
    os << ']';
}

[[nodiscard]] std::unique_ptr<expression> add_expr::eval() const noexcept{ 
    if (dynamic_cast<const num_literal*>(m_lhs) && dynamic_cast<const num_literal*>(m_rhs)) {
        const u64 left_num = static_cast<const num_literal*>(m_lhs)->value();
        const u64 right_num = static_cast<const num_literal*>(m_rhs)->value();
        return std::make_unique<num_literal>(left_num + right_num);
    }
}

void call_expr::pseudo(std::ostream &os) const noexcept {
    m_callee->pseudo(os);
    os << '(';
    for (const auto& arg : m_arguments) {
        arg->pseudo(os);
        os << ", ";
    }
    os << ')';
}

void call_expr::ast(std::ostream &os) const noexcept {
    os << "Call[callee=";
    m_callee->ast(os);
    os << ", arguments={";
    for (const auto& arg : m_arguments) {
        arg->ast(os);
        os << ',';
    }
    os << "}]";
}

void assign_expr::pseudo(std::ostream& os) const noexcept {
    m_lhs->pseudo(os);
    os << " = ";
    m_rhs->pseudo(os);
}

void assign_expr::ast(std::ostream& os) const noexcept {
    os << "assign[";
    m_lhs->ast(os);
    os << ", ";
    m_rhs->ast(os);
}

}