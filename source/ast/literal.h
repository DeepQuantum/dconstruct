#pragma once

#include "expressions.h"
#include "type.h"

namespace dconstruct::ast {

    struct literal : public expression {
        literal(const type& value) : m_value(value) {};

        const type& get_value() const noexcept { return m_value; }

        void pseudo(std::ostream& os) const override {
            os << m_value;
        }

        void ast(std::ostream& os) const override {
            os << "literal[" << m_value << ']';
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
        type m_value;
    };
}
