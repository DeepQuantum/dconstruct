#pragma once
#include "expressions.h"

namespace dconstruct::dcompiler {
    struct binary_expr : public expression {
    public:
        binary_expr(expression_uptr lhs, expression_uptr rhs)
            : m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}

        void pseudo(std::ostream&) const noexcept override;
        void ast(std::ostream&) const noexcept override;
        [[nodiscard]] b8 operator==(const expression &rhs) const noexcept override;

    protected:
        [[nodiscard]] virtual char get_op_char() const noexcept = 0;
        [[nodiscard]] virtual std::string get_op_name() const noexcept = 0;

        expression_uptr m_lhs;
        expression_uptr m_rhs;
    };

    struct add_expr : public binary_expr {
    public:
        add_expr(expression_uptr lhs, expression_uptr rhs) : binary_expr(std::move(lhs), std::move(rhs)) {};
        [[nodiscard]] expression_uptr eval() const noexcept final;
    private:
        [[nodiscard]] char get_op_char() const noexcept final { return '+'; }
        [[nodiscard]] std::string get_op_name() const noexcept final { return "add"; }
    };

    struct sub_expr : public binary_expr {
    public:
        sub_expr(expression_uptr lhs, expression_uptr rhs) : binary_expr(std::move(lhs), std::move(rhs)) {};
        expression_uptr eval() const noexcept final;

    private:
        [[nodiscard]] char get_op_char() const noexcept final { return '-'; }
        [[nodiscard]] std::string get_op_name() const noexcept final { return "sub"; }
    };

    struct mul_expr : public binary_expr {
    public:
        mul_expr(expression_uptr lhs, expression_uptr rhs) : binary_expr(std::move(lhs), std::move(rhs)) {};
        expression_uptr eval() const noexcept final;

    private:
        [[nodiscard]] char get_op_char() const noexcept final { return '*'; }
        [[nodiscard]] std::string get_op_name() const noexcept final { return "mul"; }
    };

    struct div_expr : public binary_expr {
    public:
        div_expr(expression_uptr lhs, expression_uptr rhs) : binary_expr(std::move(lhs), std::move(rhs)) {};
        expression_uptr eval() const noexcept final;
    private:
        [[nodiscard]] char get_op_char() const noexcept final { return '*'; }
        [[nodiscard]] std::string get_op_name() const noexcept final { return "mul"; }
    };

    struct assign_expr : public binary_expr {
    public:
        assign_expr(expression_uptr id, expression_uptr rhs) : binary_expr(std::move(id), std::move(rhs)) {};
        expression_uptr eval() const noexcept final;
    private:
        [[nodiscard]] char get_op_char() const noexcept final { return '='; }
        [[nodiscard]] std::string get_op_name() const noexcept final { return "assign"; }
    };

    struct compare_expr : public binary_expr {

    private:
        enum comp_type {
            LT,
            LET,
            GT,
            GET,
            EQ
        } m_compType;
    };

    template<typename expr_t>
    concept requires_binary_expr = std::is_base_of_v<binary_expr, expr_t>;
}