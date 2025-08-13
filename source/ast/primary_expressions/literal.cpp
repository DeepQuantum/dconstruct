#include "ast/primary_expressions/literal.h"


namespace dconstruct::ast {

void literal::pseudo(std::ostream& os) const {
    os << primitive_to_string(m_value);
}

void literal::ast(std::ostream& os) const {
    os << "literal[" << kind_to_string(static_cast<primitive_kind>(m_value.index()));
    pseudo(os);
    os << "]";
}


[[nodiscard]] std::unique_ptr<expression> literal::simplify() const {
    return std::make_unique<literal>(m_value);
}

[[nodiscard]] b8 literal::equals(const expression& rhs) const noexcept {
    const literal* rhs_ptr = dynamic_cast<const literal*>(&rhs);
    if (rhs_ptr != nullptr) {
        return m_value == rhs_ptr->m_value;
    }
    return false;
}

[[nodiscard]] std::unique_ptr<expression> literal::clone() const {
    return std::make_unique<literal>(m_value);
}

[[nodiscard]] std::optional<full_type> literal::compute_type(const compiler::environment&) const noexcept {
    return primitive_type { kind_from_primitive_value(m_value) };
}

}