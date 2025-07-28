#include "expressions.h"
#include "binary_expressions.h"

namespace dconstruct::ast {

[[nodiscard]] b8 identifier::equals(const expression &rhs) const noexcept {
    const identifier* rhs_id = dynamic_cast<const identifier*>(&rhs);
    if (rhs_id == nullptr) {
        return false;
    }
    return m_name == rhs_id->m_name && m_idx == rhs_id->m_idx;
}

[[nodiscard]] b8 grouping::equals(const expression &rhs) const noexcept {
    const grouping* rhs_ptr = dynamic_cast<const grouping*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_expr == rhs_ptr->m_expr;
}

[[nodiscard]] b8 logical_not_expr::equals(const expression &rhs) const noexcept {
    const logical_not_expr* rhs_ptr = dynamic_cast<const logical_not_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_lhs == rhs_ptr->m_lhs;
}

[[nodiscard]] std::unique_ptr<expression> identifier::eval() const {
    return std::make_unique<identifier>(*this);
}

[[nodiscard]] std::unique_ptr<expression> grouping::eval() const {
    return std::make_unique<grouping>(std::move(m_expr));
}

//void call_expr::pseudo(pseudo_stream &os) const noexcept {
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

void logical_not_expr::pseudo(std::ostream &os) const {
    os << '!' << m_lhs;
}

void logical_not_expr::ast(std::ostream &os) const {
    os << "logica_not[" << m_lhs << ']'; 
}

[[nodiscard]] std::unique_ptr<expression> logical_not_expr::eval() const {
    return nullptr;
}

void identifier::ast(std::ostream& os) const {
    os << "identifier[" << m_idx << ", " << m_name << ']';
}

void identifier::pseudo(std::ostream& os) const {
    os << m_name;
}

void grouping::ast(std::ostream& os) const {
    os << "grouping[" << m_expr << ']';
}

void grouping::pseudo(std::ostream& os) const {
    os << '(' << m_expr << ')';
}

}