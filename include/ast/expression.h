#pragma once

#include "base.h"
#include "printable.h"
#include "compilation/dc_register.h"
#include "ast/type.h"
#include "compilation/environment.h"
#include <ostream>
#include <vector>

namespace dconstruct::ast {

    struct expression : public Iprintable {
        virtual ~expression() = default;
        [[nodiscard]] virtual std::unique_ptr<expression> simplify() const = 0;
        [[nodiscard]] virtual b8 equals(const expression& other) const noexcept = 0;
        [[nodiscard]] virtual std::unique_ptr<expression> clone() const = 0;
        //[[nodiscard]] virtual compilation::dc_register evaluate_to_register() const noexcept;

        [[nodiscard]] inline std::optional<full_type> get_type(const compiler::environment& env) {
            if (!m_type.has_value()) {
                auto new_type = compute_type(env);
                if (!new_type.has_value()) {
                    return std::nullopt;
                }
                m_type = new_type;
            }
            return m_type;
        }

        [[nodiscard]] virtual std::optional<full_type> compute_type(const compiler::environment& env) const = 0;

        void set_type(const full_type& type) noexcept {
            m_type = type;
        }
        
    protected:
        std::optional<full_type> m_type;
    };

    [[nodiscard]] inline b8 operator==(const expression& lhs, const expression& rhs) noexcept {
        return lhs.equals(rhs);
    }

    [[nodiscard]] inline b8 operator==(const std::unique_ptr<expression>& lhs, const std::unique_ptr<expression>& rhs) noexcept {
        if (lhs == nullptr || rhs == nullptr) {
            return lhs == nullptr && rhs == nullptr;
        }
        return *lhs == *rhs;
    }

    struct unary_expr : public expression {
        unary_expr(std::unique_ptr<expression>&& rhs) noexcept : m_rhs(std::move(rhs)) {};

        // for testing ! stupid and expensive.
        [[nodiscard]] inline b8 equals(const expression& rhs) const noexcept final {
            const unary_expr* rhs_ptr = dynamic_cast<const unary_expr*>(&rhs);
            if (rhs_ptr != nullptr) {
                return typeid(*this) == typeid(*rhs_ptr) && m_rhs == rhs_ptr->m_rhs;
            }
            return false;
        }

        [[nodiscard]] inline std::optional<full_type> compute_type(const compiler::environment&) const override {
            return full_type{ primitive_type { primitive_kind::UNKNOWN } };
        }

        std::unique_ptr<expression> m_rhs;

    };

    struct binary_expr : public expression {
    public:
        binary_expr(std::unique_ptr<expression>&& lhs, std::unique_ptr<expression>&& rhs) noexcept
            : m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}

        inline void pseudo(std::ostream& os) const override {
            os << *m_lhs << ' ' << get_op_char() << ' ' << *m_rhs;
        }

        inline void ast(std::ostream& os) const override {
            os << get_op_name() << '[' << *m_lhs << ", " << *m_rhs << ']';
        }

        [[nodiscard]] inline std::optional<full_type> compute_type(const compiler::environment&) const override {
            return full_type{ primitive_type {primitive_kind::UNKNOWN} };
        }

        // for testing ! stupid and expensive.
        [[nodiscard]] inline b8 equals(const expression& rhs) const noexcept final {
            const binary_expr* rhs_ptr = dynamic_cast<const binary_expr*>(&rhs);
            if (rhs_ptr != nullptr) {
                return typeid(*this) == typeid(*rhs_ptr) && m_rhs == rhs_ptr->m_rhs && m_lhs == rhs_ptr->m_lhs;
            }
            return false;
        }

        std::unique_ptr<expression> m_lhs;
        std::unique_ptr<expression> m_rhs;

    protected:
        [[nodiscard]] virtual std::string get_op_char() const noexcept = 0;
        [[nodiscard]] virtual std::string get_op_name() const noexcept = 0;
    };


    template <typename impl_unary_expr>
    struct clonable_unary_expr : public unary_expr {
        using unary_expr::unary_expr;

        [[nodiscard]] std::unique_ptr<expression> clone() const final {
            return std::make_unique<impl_unary_expr>(m_rhs != nullptr ? m_rhs->clone() : nullptr);
        }
    };

    template <typename impl_binary_expr>
    struct clonable_binary_expr : public binary_expr {
        using binary_expr::binary_expr;

        [[nodiscard]] std::unique_ptr<expression> clone() const final {
            return std::make_unique<impl_binary_expr>(
                m_lhs != nullptr ? m_lhs->clone() : nullptr,
                m_rhs != nullptr ? m_rhs->clone() : nullptr
            );
        }
    };

    template<typename expr_t>
    concept requires_binary_expr = std::is_base_of_v<binary_expr, expr_t>;    
};