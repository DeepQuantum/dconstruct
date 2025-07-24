#include "expressions.h"
#include "binary_expressions.h"

namespace dconstruct::ast {


std::unique_ptr<expression> string_literal::eval() const noexcept {
    return std::make_unique<string_literal>(*this);
}

std::unique_ptr<expression> num_literal::eval() const noexcept {
    return std::make_unique<num_literal>(*this);
}

template<typename T>
[[nodiscard]] b8 literal<T>::operator==(const expression &rhs) const noexcept {
    const literal<T>* rhs_lit = dynamic_cast<const literal<T>*>(&rhs);
    if (rhs_lit == nullptr) {
        return false;
    }
    return m_value == rhs_lit->m_value;
}

[[nodiscard]] b8 identifier::operator==(const expression &rhs) const noexcept {
    const identifier* rhs_id = dynamic_cast<const identifier*>(&rhs);
    if (rhs_id == nullptr) {
        return false;
    }
    return m_name == rhs_id->m_name && m_idx == rhs_id->m_idx;
}

[[nodiscard]] std::unique_ptr<expression> identifier::eval() const noexcept {
    return std::make_unique<identifier>(*this);
}

//void call_expr::pseudo(std::ostream &os) const noexcept {
//    m_callee->pseudo(os);
//    os << '(';
//    for (const auto& arg : m_arguments) {
//        arg->pseudo(os);
//        os << ", ";
//    }
//    os << ')';
//}
//
//void call_expr::ast(std::ostream &os) const noexcept {
//    os << "call[callee=";
//    m_callee->ast(os);
//    os << ", arguments={";
//    for (const auto& arg : m_arguments) {
//        arg->ast(os);
//        os << ',';
//    }
//    os << "}]";
//}

void num_literal::pseudo(std::ostream& os) const noexcept {
    os << std::to_string(m_value);
}

void num_literal::ast(std::ostream& os) const noexcept {
    os << "num_literal[";
    os << std::to_string(m_value);
    os << "]";
}

void string_literal::pseudo(std::ostream& os) const noexcept {
    os << '"';
    os << m_value;
    os << '"';
}

void string_literal::ast(std::ostream& os) const noexcept {
    os << "string_literal[";
    os << m_value;
    os << "]";
}

void identifier::ast(std::ostream& os) const noexcept {
    os << "identifier[";
    os << std::to_string(m_idx);
    os << ", ";
    os << m_name;
    os << "]";
}

void identifier::pseudo(std::ostream& os) const noexcept {
    os << m_name;
}

}