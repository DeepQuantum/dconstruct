#pragma once

#include "base.h"
#include "printable.h"
#include <ostream>
#include <vector>

namespace dconstruct::ast {

    struct expression : public Iprintable {
        virtual ~expression() = default;
        [[nodiscard]] virtual std::unique_ptr<expression> eval() const = 0;
        [[nodiscard]] virtual b8 equals(const expression& other) const noexcept = 0;
    };

    [[nodiscard]] inline b8 operator==(const expression& lhs, const expression& rhs) noexcept {
        return lhs.equals(rhs);
    }

    [[nodiscard]] inline b8 operator==(const std::unique_ptr<expression>& lhs, const std::unique_ptr<expression>& rhs) noexcept {
        if (lhs == nullptr || rhs == nullptr) {
            return lhs == rhs;
        }
        return *lhs == *rhs;
    }

    struct unary_expr : public expression {
        unary_expr(std::unique_ptr<expression> lhs) : m_rhs(std::move(lhs)) {};

    protected:
        std::unique_ptr<expression> m_rhs;
    };

    struct binary_expr : public expression {
    public:
        binary_expr(std::unique_ptr<expression> lhs, std::unique_ptr<expression> rhs)
            : m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}

        inline void pseudo(std::ostream& os) const override {
            os << *m_lhs << ' ' << get_op_char() << ' ' << *m_rhs;
        }

        inline void ast(std::ostream& os) const override {
            os << get_op_name() << '[' << *m_lhs << ", " << *m_rhs << ']';
        }

    protected:
        [[nodiscard]] virtual std::string get_op_char() const noexcept = 0;
        [[nodiscard]] virtual std::string get_op_name() const noexcept = 0;

        std::unique_ptr<expression> m_lhs;
        std::unique_ptr<expression> m_rhs;
    };

    template<typename expr_t>
    concept requires_binary_expr = std::is_base_of_v<binary_expr, expr_t>;    
};