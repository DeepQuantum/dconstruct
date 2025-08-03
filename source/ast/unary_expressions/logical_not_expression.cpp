#include "logical_not_expression.h"
#include "literal.h"

namespace dconstruct::ast {
void logical_not_expr::pseudo(std::ostream &os) const {
    os << '!' << m_rhs;
}

void logical_not_expr::ast(std::ostream &os) const {
    os << "logical_not[" << m_rhs << ']'; 
}

[[nodiscard]] b8 logical_not_expr::equals(const expression &rhs) const noexcept {
    const logical_not_expr* rhs_ptr = dynamic_cast<const logical_not_expr*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_rhs == rhs_ptr->m_rhs;
}

[[nodiscard]] std::unique_ptr<expression> logical_not_expr::eval() const {
    std::unique_ptr<expression> rhs = m_rhs->eval();
    const literal* rhs_ptr = dynamic_cast<const literal*>(rhs.get());
    if (rhs_ptr != nullptr) {
        const primitive_value_type value = rhs_ptr->get_value();
        switch(rhs_ptr->get_type()) {
            case TK_STRUCT:
            case TK_ENUM:
            case TK_SID:
            case TK_PTR:
            case TK_UNKNOWN: return nullptr;
            case TK_BOOL: return std::make_unique<literal>(!std::get<b8>(value));
            case TK_STRING: return std::make_unique<literal>(!(std::get<std::string>(value).empty()));
            case TK_NULL: return std::make_unique<literal>(true);
            default: {
                const std::optional<primitive_number_type> num_opt = get_number(value);
                if (num_opt.has_value()) {
                    return std::make_unique<literal>(std::visit([](auto&& arg) -> b8 {
                        return arg == 0;
                    }, num_opt.value()));
                } 
                return nullptr;
            }
        }
    }
    return std::make_unique<logical_not_expr>(*this);
}
}