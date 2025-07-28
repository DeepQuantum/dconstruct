#include "binary_expressions.h"
#include "literal.h"

namespace dconstruct::ast {

void binary_expr::pseudo(std::ostream& os) const {
    os << *m_lhs << ' ' << get_op_char() << ' ' << *m_rhs;
}

void binary_expr::ast(std::ostream& os) const {
    os << get_op_name() << '[' << *m_lhs << ", " << *m_rhs << ']';
}

[[nodiscard]] b8 add_expr::equals(const expression& rhs) const noexcept {
    const add_expr* rhs_ptr = dynamic_cast<const add_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_lhs == rhs_ptr->m_lhs && m_rhs == rhs_ptr->m_rhs;
}

[[nodiscard]] b8 sub_expr::equals(const expression& rhs) const noexcept {
    const sub_expr* rhs_ptr = dynamic_cast<const sub_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_lhs == rhs_ptr->m_lhs && m_rhs == rhs_ptr->m_rhs;
}

[[nodiscard]] b8 mul_expr::equals(const expression& rhs) const noexcept {
    const mul_expr* rhs_ptr = dynamic_cast<const mul_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_lhs == rhs_ptr->m_lhs && m_rhs == rhs_ptr->m_rhs;
}

[[nodiscard]] b8 div_expr::equals(const expression& rhs) const noexcept {
    const div_expr* rhs_ptr = dynamic_cast<const div_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_lhs == rhs_ptr->m_lhs && m_rhs == rhs_ptr->m_rhs;
}

[[nodiscard]] b8 assign_expr::equals(const expression& rhs) const noexcept {
    const assign_expr* rhs_ptr = dynamic_cast<const assign_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_lhs == rhs_ptr->m_lhs && m_rhs == rhs_ptr->m_rhs;
}

[[nodiscard]] b8 compare_expr::equals(const expression& rhs) const noexcept {
    const compare_expr* rhs_ptr = dynamic_cast<const compare_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_lhs == rhs_ptr->m_lhs && m_rhs == rhs_ptr->m_rhs && m_compType == rhs_ptr->m_compType;
}

[[nodiscard]] std::unique_ptr<expression> add_expr::eval() const { 
    if (const literal<u64>* lhs_num_lit = dynamic_cast<const literal<u64>*>(m_lhs.get())) {
        if (const literal<u64>* rhs_num_lit = dynamic_cast<const literal<u64>*>(m_rhs.get())) {
            const u64 left_num = lhs_num_lit->get_value();
            const u64 right_num = rhs_num_lit->get_value();
            return std::make_unique<literal<u64>>(left_num + right_num);
        }
    }
    return nullptr;
}

[[nodiscard]] std::unique_ptr<expression> mul_expr::eval() const{ 
    return nullptr;
}

[[nodiscard]] std::unique_ptr<expression> sub_expr::eval() const{ 
    return nullptr;
}

[[nodiscard]] std::unique_ptr<expression> div_expr::eval() const { 
    return nullptr;
}

[[nodiscard]] std::unique_ptr<expression> assign_expr::eval() const {
    return nullptr;
}

[[nodiscard]] std::unique_ptr<expression> compare_expr::eval() const {
    return nullptr;
}
}