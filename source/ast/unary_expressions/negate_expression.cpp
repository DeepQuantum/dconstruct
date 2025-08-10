#include "ast/unary_expressions/negate_expression.h"
#include "ast/primary_expressions/literal.h"

namespace dconstruct::ast {

void negate_expression::pseudo(std::ostream& os) const {
    os << "-" << m_rhs;
}

void negate_expression::ast(std::ostream& os) const {
    os << "negate[" << m_rhs << ']';
}

// [[nodiscard]] b8 negate_expression::equals(const expression& rhs) const noexcept {
//     const negate_expression* rhs_ptr = dynamic_cast<const negate_expression*>(&rhs);
//     if (rhs_ptr == nullptr) {
//         return false;
//     }
//     return m_rhs == rhs_ptr->m_rhs;
// }

[[nodiscard]] std::unique_ptr<expression> negate_expression::simplify() const {
    std::unique_ptr<expression> rhs = m_rhs->simplify();
    const literal* rhs_ptr = dynamic_cast<const literal*>(rhs.get());
    if (rhs_ptr != nullptr) {
        const primitive_value_type prim = rhs_ptr->get_value();
        const std::optional<primitive_number_type> option = get_number(prim);
        if (option.has_value()) {
            return std::make_unique<literal>(std::visit([](auto&& arg) -> primitive_value_type {
                return -static_cast<std::decay_t<decltype(arg)>>(arg);
            }, option.value()));
        }
        return nullptr;
    }
    return std::make_unique<negate_expression>(std::move(rhs));
}

}

