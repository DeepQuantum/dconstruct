#include "ast/unary_expressions/logical_not_expression.h"
#include "ast/primary_expressions/literal.h"

namespace dconstruct::ast {
void logical_not_expr::pseudo(std::ostream &os) const {
    os << '!' << m_rhs;
}

void logical_not_expr::ast(std::ostream &os) const {
    os << "logical_not[" << m_rhs << ']'; 
}

// [[nodiscard]] b8 logical_not_expr::equals(const expression &rhs) const noexcept {
//     const logical_not_expr* rhs_ptr = dynamic_cast<const logical_not_expr*>(&rhs);
//     if (rhs_ptr == nullptr) {
//         return false;
//     }
//     return m_rhs == rhs_ptr->m_rhs;
// }

[[nodiscard]] std::unique_ptr<expression> logical_not_expr::simplify() const {
    std::unique_ptr<expression> rhs = m_rhs->simplify();
    const literal* rhs_ptr = dynamic_cast<const literal*>(rhs.get());
    if (rhs_ptr != nullptr) {
        const primitive_value value = rhs_ptr->m_value;
        switch(rhs_ptr->get_type()) {
            case primitive_kind::SID:
            case primitive_kind::UNKNOWN: return nullptr;
            case primitive_kind::BOOL:    return std::make_unique<literal>(!std::get<b8>(value));
            case primitive_kind::STRING:  return std::make_unique<literal>(!(std::get<std::string>(value).empty()));
            case primitive_kind::_NULL:   return std::make_unique<literal>(true);
            default: {
                const std::optional<primitive_number> num_opt = get_number(value);
                if (num_opt.has_value()) {
                    return std::make_unique<literal>(std::visit([](auto&& arg) -> b8 {
                        return arg == 0;
                    }, num_opt.value()));
                } 
                return nullptr;
            }
        }
    }
    return std::make_unique<logical_not_expr>(std::move(rhs));
}
}