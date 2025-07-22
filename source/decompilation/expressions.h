#pragma once

#include "base.h"
#include <vector>
#include <ostream>

namespace dconstruct::dcompiler {

    struct expression {
    public:
        virtual void pseudo(std::ostream&) const noexcept = 0;
        virtual void ast(std::ostream&) const noexcept = 0;
        [[nodiscard]] virtual std::unique_ptr<const expression> eval() const noexcept = 0;
        [[nodiscard]] virtual b8 operator==(const expression &rhs) const noexcept = 0;
    };

    using expression_uptr = std::unique_ptr<const expression>;

    struct unary_expr : public expression {
    public:
        unary_expr(expression_uptr lhs) : m_lhs(std::move(lhs)) {};

    protected:
        expression_uptr m_lhs;
    };

    struct call_expr : public expression {
    private:
        expression_uptr m_callee;
        std::vector<expression_uptr> m_arguments;

        void pseudo(std::ostream &os) const noexcept final;
        void ast(std::ostream &os) const noexcept final;
    };

    template<typename T>
    struct literal : public expression {
        const T& get_value() const noexcept { return m_value; }
        [[nodiscard]] b8 operator==(const expression &rhs) const noexcept override;
    protected:
        T m_value;
    };

    struct num_literal : public literal<u64> {
        num_literal(const u64 num)  {
            m_value = num;
        }

        void pseudo(std::ostream& os) const noexcept final;
        void ast(std::ostream& os) const noexcept final;
        [[nodiscard]] expression_uptr eval() const noexcept final;
    };
    
    struct string_literal : public literal<std::string> {
        void pseudo(std::ostream& os) const noexcept final;
        void ast(std::ostream& os) const noexcept final;
        [[nodiscard]] expression_uptr eval() const noexcept final;
    };

    struct identifier : public expression {
        identifier(const std::string &name, const u8 idx) : m_name(name), m_idx(idx) {};
        void pseudo(std::ostream& os) const noexcept final;
        void ast(std::ostream& os) const noexcept final;
        [[nodiscard]] expression_uptr eval() const noexcept final;
        [[nodiscard]] b8 operator==(const expression &rhs) const noexcept override;


    private:
        std::string m_name;
        u8 m_idx;        
    };
};