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

[[nodiscard]] b8 binary_expr::operator==(const expression &rhs) const noexcept {
    const binary_expr* rhs_bin = dynamic_cast<const binary_expr*>(&rhs);
    if (rhs_bin == nullptr) {
        return false;
    }
    return m_lhs == rhs_bin->m_lhs && m_rhs == rhs_bin->m_rhs;
}

[[nodiscard]] expression_uptr add_expr::eval() const noexcept{ 
    if (const num_literal* lhs_num_lit = dynamic_cast<const num_literal*>(m_lhs.get())) {
        if (const num_literal* rhs_num_lit = dynamic_cast<const num_literal*>(m_rhs.get())) {
            const u64 left_num = lhs_num_lit->get_value();
            const u64 right_num = rhs_num_lit->get_value();
            return std::make_unique<num_literal>(left_num + right_num);
        }
    }
    return nullptr;
}

[[nodiscard]] expression_uptr mul_expr::eval() const noexcept{ 
    return nullptr;
}

[[nodiscard]] expression_uptr sub_expr::eval() const noexcept{ 
    return nullptr;
}

[[nodiscard]] expression_uptr div_expr::eval() const noexcept{ 
    return nullptr;
}

[[nodiscard]] expression_uptr assign_expr::eval() const noexcept {
    return nullptr;
}
}