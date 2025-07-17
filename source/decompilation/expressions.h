#pragma once

#include "base.h"
#include <vector>
#include <ostream>

namespace dconstruct::dcompiler {

    struct expression {
    public:
        virtual void pseudo(std::ostream&) const noexcept = 0;
        virtual void ast(std::ostream&) const noexcept = 0;
        virtual std::unique_ptr<expression> eval() const noexcept = 0;
    };

    struct binary_expr : public expression {
    public:
        binary_expr(const expression *lhs, const expression *rhs) : m_lhs(lhs), m_rhs(rhs) {};

    protected:
        const expression *m_lhs;
        const expression *m_rhs;
    };

    struct unary_expr : public expression {
    public:
        unary_expr(const expression *lhs) : m_lhs(lhs) {};

    protected:
        const expression *m_lhs;
    };

    struct add_expr : public binary_expr {
    public:
        add_expr(const expression *lhs, const expression *rhs) : binary_expr(lhs, rhs) {};

        void pseudo(std::ostream& os) const noexcept final;
        void ast(std::ostream& os) const noexcept final;
        std::unique_ptr<expression> eval() const noexcept final;
    };

    struct call_expr : public expression {
    private:
        const expression* m_callee;
        std::vector<const expression*> m_arguments;

        void pseudo(std::ostream &os) const noexcept final;
        void ast(std::ostream &os) const noexcept final;
    };

    struct assign_expr : public binary_expr {
        void pseudo(std::ostream& os) const noexcept final;
        void ast(std::ostream& os) const noexcept final;
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

    template<typename T>
    struct literal : public expression {
        const T& value() const noexcept;
    protected:
        T m_value;
    };

    struct num_literal : public literal<u64> {
        num_literal(const u64 num)  {
            m_value = num;
        }

        void pseudo(std::ostream& os) const noexcept final;
        void ast(std::ostream& os) const noexcept final;
        std::unique_ptr<expression> eval() const noexcept final;
    };
    
    struct string_literal : public literal<std::string> {
        void pseudo(std::ostream& os) const noexcept final;
        void ast(std::ostream& os) const noexcept final;
        std::unique_ptr<expression> eval() const noexcept final;
    };
};