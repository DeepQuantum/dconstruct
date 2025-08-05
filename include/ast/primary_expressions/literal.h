#pragma once

#include "ast/expression.h"
#include "ast/type.h"

namespace dconstruct::ast {

    struct literal : public expression {
    
        template<typename T>
        literal(T&& value) : m_value(std::forward<T>(value)) {};

        [[nodiscard]] inline const primitive_value_type& get_value() const noexcept { return m_value; }

        [[nodiscard]] inline type_kind get_type() const noexcept {
            return kind_from_primitive_value(m_value);
        }

        
        void pseudo(std::ostream& os) const override;

        void ast(std::ostream& os) const override;
    
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
