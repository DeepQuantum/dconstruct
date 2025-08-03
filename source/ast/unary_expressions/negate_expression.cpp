#include "negate_expression.h"
#include "literal.h"

namespace dconstruct::ast {

void negate_expression::pseudo(std::ostream& os) const {
    os << "-" << m_rhs;
}

void negate_expression::ast(std::ostream& os) const {
    os << "negate[" << m_rhs << ']';
}

[[nodiscard]] b8 negate_expression::equals(const expression& rhs) const  {
    const negate_expression* rhs_ptr = dynamic_cast<const negate_expression*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_rhs == rhs_ptr->m_rhs;
}

[[nodiscard]] std::unique_ptr<expression> negate_expression::eval() const {
    std::unique_ptr<expression> rhs = m_rhs->eval();
    const literal* rhs_ptr = dynamic_cast<const literal*>(rhs.get());
    if (rhs_ptr != nullptr) {
        const primitive_value_type prim = rhs_ptr->get_value();
        const std::optional<primitive_number_type> option = get_number(prim);
        if (option.has_value()) {
            return std::make_unique<literal>(std::visit([](auto&& arg) {
                return -arg;
            }, option.value()));
        }
        return nullptr;
    }
    return std::make_unique<negate_expression>(*this);
}

}

