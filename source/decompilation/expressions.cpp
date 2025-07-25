#include "expressions.h"
#include "binary_expressions.h"

namespace dconstruct::ast {

[[nodiscard]] b8 identifier::operator==(const expression &rhs) const noexcept {
    const identifier* rhs_id = dynamic_cast<const identifier*>(&rhs);
    if (rhs_id == nullptr) {
        return false;
    }
    return m_name == rhs_id->m_name && m_idx == rhs_id->m_idx;
}

[[nodiscard]] b8 grouping::operator==(const expression &rhs) const noexcept {
    const grouping* rhs_group = dynamic_cast<const grouping*>(&rhs);
    if (rhs_group == nullptr) {
        return false;
    }
    return rhs_group->m_expr == m_expr;
}

[[nodiscard]] b8 unary_expr::operator==(const expression &rhs) const noexcept {
    const unary_expr* rhs_group = dynamic_cast<const unary_expr*>(&rhs);
    if (rhs_group == nullptr) {
        return false;
    }
    return rhs_group->m_lhs == m_lhs;
}

[[nodiscard]] std::unique_ptr<expression> identifier::eval() const noexcept {
    return std::make_unique<identifier>(*this);
}

[[nodiscard]] std::unique_ptr<expression> grouping::eval() const noexcept {
    return std::make_unique<grouping>(std::move(m_expr));
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

void logical_not_expr::pseudo(std::ostream &os) const noexcept {
    os << '!'; 
    m_lhs->pseudo(os);
}

void logical_not_expr::ast(std::ostream &os) const noexcept {
    os << "logica_not["; 
    m_lhs->pseudo(os);
    os << ']'; 
}

[[nodiscard]] std::unique_ptr<expression> logical_not_expr::eval() const noexcept {
    return nullptr;
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

void grouping::ast(std::ostream& os) const noexcept {
    os << "grouping[";
    m_expr->ast(os);
    os << ']';
}

void grouping::pseudo(std::ostream& os) const noexcept {
    os << "(";
    m_expr->pseudo(os);
    os << ')';
}

}