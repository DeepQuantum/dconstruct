#include "ast/primary_expressions/literal.h"


namespace dconstruct::ast {

void literal::pseudo(std::ostream& os) const {
    std::visit([&](auto&& arg) -> void {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            os << arg;
        }
        else if constexpr (std::is_same_v<T, char>) {
            os << std::string(1, arg);
        }
        else if constexpr (std::is_same_v<T, b8>) {
            os << arg ? "true" : "false";
        }
        else if constexpr (std::is_same_v<T, std::nullptr_t>) {
            os << "null";
        }
        else if constexpr (std::is_same_v<T, sid_literal_type>) {
            os << "#" + std::get<1>(arg);
        }
        else {
            os << std::to_string(arg);
        }
    }, m_value);
}

void literal::ast(std::ostream& os) const {
    os << "literal[" << kind_to_string(static_cast<type_kind>(m_value.index()));
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

}