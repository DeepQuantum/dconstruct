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

        void pseudo(std::ostream& os) const final;

        void ast(std::ostream& os) const final;

        [[nodiscard]] std::unique_ptr<expression> eval() const final;
        [[nodiscard]] b8 equals(const expression& other) const noexcept final;
        [[nodiscard]] std::unique_ptr<expression> clone() const final;

    protected:
        primitive_value_type m_value;
    };
}
