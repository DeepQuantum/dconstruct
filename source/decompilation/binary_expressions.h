#pragma once
#include "expressions.h"

namespace dconstruct::ast {
    struct binary_expr : public expression {
    public:
        binary_expr(std::unique_ptr<expression> lhs, std::unique_ptr<expression> rhs)
            : m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}

        void pseudo(std::ostream&) const noexcept override;
        void ast(std::ostream&) const noexcept override;
        [[nodiscard]] virtual b8 operator==(const expression &rhs) const noexcept override;

    protected:
        [[nodiscard]] virtual std::string get_op_char() const noexcept = 0;
        [[nodiscard]] virtual std::string get_op_name() const noexcept = 0;

        std::unique_ptr<expression> m_lhs;
        std::unique_ptr<expression> m_rhs;
    };

    struct add_expr : public binary_expr {
    public:
        add_expr(std::unique_ptr<expression> lhs, std::unique_ptr<expression> rhs) : binary_expr(std::move(lhs), std::move(rhs)) {};
        [[nodiscard]] std::unique_ptr<expression> eval() const noexcept final;

    private:
        [[nodiscard]] std::string get_op_char() const noexcept final { return "+"; }
        [[nodiscard]] std::string get_op_name() const noexcept final { return "add"; }
    };

    struct sub_expr : public binary_expr {
    public:
        sub_expr(std::unique_ptr<expression> lhs, std::unique_ptr<expression> rhs) : binary_expr(std::move(lhs), std::move(rhs)) {};
        std::unique_ptr<expression> eval() const noexcept final;


    private:
        [[nodiscard]] std::string get_op_char() const noexcept final { return "-"; }
        [[nodiscard]] std::string get_op_name() const noexcept final { return "sub"; }
    };

    struct mul_expr : public binary_expr {
    public:
        mul_expr(std::unique_ptr<expression> lhs, std::unique_ptr<expression> rhs) : binary_expr(std::move(lhs), std::move(rhs)) {};
        std::unique_ptr<expression> eval() const noexcept final;


    private:
        [[nodiscard]] std::string get_op_char() const noexcept final { return "*"; }
        [[nodiscard]] std::string get_op_name() const noexcept final { return "mul"; }
    };

    struct div_expr : public binary_expr {
    public:
        div_expr(std::unique_ptr<expression> lhs, std::unique_ptr<expression> rhs) : binary_expr(std::move(lhs), std::move(rhs)) {};
        std::unique_ptr<expression> eval() const noexcept final;

    private:
        [[nodiscard]] std::string get_op_char() const noexcept final { return "/"; }
        [[nodiscard]] std::string get_op_name() const noexcept final { return "div"; }
    };

    struct assign_expr : public binary_expr {
    public:
        assign_expr(std::unique_ptr<expression> id, std::unique_ptr<expression> rhs) : binary_expr(std::move(id), std::move(rhs)) {};
        std::unique_ptr<expression> eval() const noexcept final;

    private:
        [[nodiscard]] std::string get_op_char() const noexcept final { return "="; }
        [[nodiscard]] std::string get_op_name() const noexcept final { return "assign"; }
    };

    struct compare_expr : public binary_expr {

        enum comp_type {
            LT,
            LET,
            GT,
            GET,
            EQ,
            NEQ
        } m_compType;

        compare_expr(const comp_type type, std::unique_ptr<expression> lhs, std::unique_ptr<expression> rhs) 
            : binary_expr(std::move(lhs), std::move(rhs)), m_compType(type) {};
        std::unique_ptr<expression> eval() const noexcept final;
        [[nodiscard]] std::string get_op_char() const noexcept final { 
            switch (m_compType) {
                case LT: return "<";
                case LET: return "<=";
                case GT: return ">";
                case GET: return ">=";
                case EQ: return "==";
                case NEQ: return "!=";
                default: return "==";
            }
        }
        [[nodiscard]] std::string get_op_name() const noexcept final { return "comp" + get_op_char(); }
    };

    template<typename expr_t>
    concept requires_binary_expr = std::is_base_of_v<binary_expr, expr_t>;
}