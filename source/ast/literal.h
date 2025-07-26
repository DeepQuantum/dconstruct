#pragma once

#include "expressions.h"

namespace dconstruct::ast {
template<typename T>
struct literal : public expression {
    literal(const T& value) : m_value(value) {};

    const T& get_value() const noexcept { return m_value; }

    void pseudo(std::ostream& os) const noexcept override {
        os << m_value;
    }

    void ast(std::ostream& os) const noexcept override {
        os << "literal[";
        os << m_value;
        os << "]";
    }
    
    [[nodiscard]] std::unique_ptr<expression> eval() const noexcept override {
        return std::make_unique<literal<T>>(*this);
    }

    [[nodiscard]] b8 operator==(const expression &rhs) const noexcept override {
        const literal<T>* rhs_lit = dynamic_cast<const literal<T>*>(&rhs);
        if (rhs_lit == nullptr) {
            return false;
        }
        return m_value == rhs_lit->m_value;
    }
protected:
    T m_value;
};
}
