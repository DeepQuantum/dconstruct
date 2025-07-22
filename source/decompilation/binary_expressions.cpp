#include "binary_expressions.h"

namespace dconstruct::dcompiler {

void binary_expr::pseudo(std::ostream& os) const noexcept {
    m_lhs->pseudo(os);
    os << ' ' << get_op_char() << ' ';
    m_rhs->pseudo(os);
}

void binary_expr::ast(std::ostream& os) const noexcept {
    os << get_op_name() << '[';
    m_lhs->ast(os);
    os << ", ";
    m_rhs->ast(os);
    os << ']';
}

[[nodiscard]] b8 add_expr::operator==(const expression& rhs) const noexcept {
    if (const add_expr* rhs_add = dynamic_cast<const add_expr*>(&rhs)) {
        return m_lhs == rhs_add->m_lhs && m_rhs == rhs_add->m_rhs;
    }
    return false;
}

[[nodiscard]] b8 sub_expr::operator==(const expression& rhs) const noexcept {
    if (const sub_expr* rhs_add = dynamic_cast<const sub_expr*>(&rhs)) {
        return m_lhs == rhs_add->m_lhs && m_rhs == rhs_add->m_rhs;
    }
    return false;
}
[[nodiscard]] b8 mul_expr::operator==(const expression& rhs) const noexcept {
    if (const mul_expr* rhs_add = dynamic_cast<const mul_expr*>(&rhs)) {
        return m_lhs == rhs_add->m_lhs && m_rhs == rhs_add->m_rhs;
    }
    return false;
}
[[nodiscard]] b8 div_expr::operator==(const expression& rhs) const noexcept {
    if (const div_expr* rhs_add = dynamic_cast<const div_expr*>(&rhs)) {
        return m_lhs == rhs_add->m_lhs && m_rhs == rhs_add->m_rhs;
    }
    return false;
}

[[nodiscard]] b8 assign_expr::operator==(const expression& rhs) const noexcept {
    if (const assign_expr* rhs_add = dynamic_cast<const assign_expr*>(&rhs)) {
        return m_lhs == rhs_add->m_lhs && m_rhs == rhs_add->m_rhs;
    }
    return false;
}


[[nodiscard]] std::unique_ptr<expression> add_expr::eval() const noexcept{ 
    if (const num_literal* lhs_num_lit = dynamic_cast<const num_literal*>(m_lhs.get())) {
        if (const num_literal* rhs_num_lit = dynamic_cast<const num_literal*>(m_rhs.get())) {
            const u64 left_num = lhs_num_lit->get_value();
            const u64 right_num = rhs_num_lit->get_value();
            return std::make_unique<num_literal>(left_num + right_num);
        }
    }
    return nullptr;
}

[[nodiscard]] std::unique_ptr<expression> mul_expr::eval() const noexcept{ 
    return nullptr;
}

[[nodiscard]] std::unique_ptr<expression> sub_expr::eval() const noexcept{ 
    return nullptr;
}

[[nodiscard]] std::unique_ptr<expression> div_expr::eval() const noexcept{ 
    return nullptr;
}

[[nodiscard]] std::unique_ptr<expression> assign_expr::eval() const noexcept {
    return nullptr;
}
}