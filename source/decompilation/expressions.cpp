#include "expressions.h"

namespace dconstruct::dcompiler {

void add_expr::pseudo(std::ostream& os) const noexcept {
    m_lhs->pseudo(os);
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

[[nodiscard]] std::unique_ptr<const expression> add_expr::eval() const noexcept{ 
    if (dynamic_cast<const num_literal*>(m_lhs.get()) && dynamic_cast<const num_literal*>(m_rhs.get())) {
        const u64 left_num = static_cast<const num_literal*>(m_lhs.get())->value();
        const u64 right_num = static_cast<const num_literal*>(m_rhs.get())->value();
        return std::make_unique<num_literal>(left_num + right_num);
    }
    return nullptr;
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
    os << "call[callee=";
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

std::unique_ptr<const expression> num_literal::eval() const noexcept {
    return std::make_unique<num_literal>(*this);
}

std::unique_ptr<const expression> string_literal::eval() const noexcept {
    return std::make_unique<string_literal>(*this);
}

template<typename T>
const T& literal<T>::value() const noexcept {
    return m_value;
}

void num_literal::pseudo(std::ostream& os) const noexcept {
    os << std::to_string(m_value);
}

void num_literal::ast(std::ostream& os) const noexcept {
    os << "num_literal[";
    os << std::to_string(m_value);
    os << "]";
}

void string_literal::pseudo(std::ostream& os) const noexcept {
    os << m_value;
}

void string_literal::ast(std::ostream& os) const noexcept {
    os << "string_literal[";
    os << m_value;
    os << "]";
}

}