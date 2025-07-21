#pragma once

#include "base.h"
#include <vector>
#include <ostream>

namespace dconstruct::dcompiler {

    

    struct expression {
    public:
        virtual void pseudo(std::ostream&) const noexcept = 0;
        virtual void ast(std::ostream&) const noexcept = 0;
        virtual std::unique_ptr<const expression> eval() const noexcept = 0;
    };

    

    struct unary_expr : public expression {
    public:
        unary_expr(std::unique_ptr<const expression> lhs) : m_lhs(std::move(lhs)) {};

    protected:
        std::unique_ptr<const expression>m_lhs;
    };

    struct call_expr : public expression {
    private:
        const expression* m_callee;
        std::vector<const expression*> m_arguments;

        void pseudo(std::ostream &os) const noexcept final;
        void ast(std::ostream &os) const noexcept final;
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
        std::unique_ptr<const expression> eval() const noexcept final;
    };
    
    struct string_literal : public literal<std::string> {
        void pseudo(std::ostream& os) const noexcept final;
        void ast(std::ostream& os) const noexcept final;
        std::unique_ptr<const expression> eval() const noexcept final;
    };
};