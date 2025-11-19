#pragma once

#include "ast/expression.h"
#include "ast/type.h"

namespace dconstruct::ast {

    struct literal : public expression {
    
        template<typename T>
        literal(T&& value) : m_value(std::forward<T>(value)) {};

        [[nodiscard]] inline primitive_kind get_type() const noexcept {
            return kind_from_primitive_value(m_value);
        }

        void pseudo_c(std::ostream& os) const final;

        void pseudo_py(std::ostream& os) const final;

        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] bool equals(const expression& other) const noexcept final;
        [[nodiscard]] expr_uptr clone() const final;
        [[nodiscard]] full_type compute_type(const compiler::environment& env) const noexcept final;
        [[nodiscard]] u16 complexity() const noexcept final;

        primitive_value m_value;
    };
}
