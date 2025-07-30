#pragma once

#include "expressions.h"
#include "type.h"

namespace dconstruct::ast {

    struct literal : public expression {
        

        template<typename T>
        literal(T&& value) : m_value(std::forward<T>(value)) {};

        const primitive_value_type& get_value() const noexcept { return m_value; }

        void pseudo(std::ostream& os) const override {
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

        void ast(std::ostream& os) const override {
            os << "literal[";
            std::visit([&](auto&& arg) -> void {
                os << kind_to_string(static_cast<type_kind>(m_value.index())) << ": ";
                pseudo(os);
            }, m_value);
            os << "]";
        }
    
        [[nodiscard]] std::unique_ptr<expression> eval() const override {
            return std::make_unique<literal>(*this);
        }

        [[nodiscard]] b8 equals(const expression &rhs) const noexcept override {
            const literal* rhs_lit = dynamic_cast<const literal*>(&rhs);
            if (rhs_lit == nullptr) {
                return false;
            }
            return m_value == rhs_lit->m_value;
        }
    protected:
        primitive_value_type m_value;
    };
}
