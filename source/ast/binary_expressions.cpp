#include "binary_expressions.h"
#include "literal.h"

namespace dconstruct::ast {

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

[[nodiscard]] b8 binary_expr::operator==(const expression& rhs) const noexcept {
    if (typeid(*this) == typeid(rhs)) {
        if (const binary_expr* rhs_add = dynamic_cast<const binary_expr*>(&rhs)) {
            return *m_lhs == *rhs_add->m_lhs && *m_rhs == *rhs_add->m_rhs;
        }
    }
    return false;
}


[[nodiscard]] std::unique_ptr<expression> add_expr::eval() const noexcept{ 
    if (const literal<u64>* lhs_num_lit = dynamic_cast<const literal<u64>*>(m_lhs.get())) {
        if (const literal<u64>* rhs_num_lit = dynamic_cast<const literal<u64>*>(m_rhs.get())) {
            const u64 left_num = lhs_num_lit->get_value();
            const u64 right_num = rhs_num_lit->get_value();
            return std::make_unique<literal<u64>>(left_num + right_num);
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

[[nodiscard]] std::unique_ptr<expression> compare_expr::eval() const noexcept {
    return nullptr;
}
}