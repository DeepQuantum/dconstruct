#include "literal.h"

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
        else if constexpr (std::is_same_v<T, nullptr_t>) {
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
    os << "literal[";
    std::visit([&](auto&& arg) -> void {
        os << kind_to_string(static_cast<type_kind>(m_value.index())) << ": ";
        pseudo(os);
    }, m_value);
    os << "]";
}

}