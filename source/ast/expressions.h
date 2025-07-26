#pragma once

#include "base.h"
#include <vector>
#include <ostream>

namespace dconstruct::ast {

    struct expression {
        virtual ~expression() = default;
        virtual void pseudo(std::ostream&) const noexcept = 0;
        virtual void ast(std::ostream&) const noexcept = 0;
        [[nodiscard]] virtual std::unique_ptr<expression> eval() const noexcept = 0;
        [[nodiscard]] virtual b8 operator==(const expression &rhs) const noexcept = 0;
    };

    inline std::ostream& operator<<(std::ostream& os, const expression &expr) {
        expr.ast(os);
        return os;
    }

    struct unary_expr : public expression {
        unary_expr(std::unique_ptr<expression> lhs) : m_lhs(std::move(lhs)) {};
        [[nodiscard]] b8 operator==(const expression &rhs) const noexcept override;

    protected:
        std::unique_ptr<expression> m_lhs;
    };

    struct logical_not_expr : public unary_expr {
        using unary_expr::unary_expr;
        
        void pseudo(std::ostream& os) const noexcept final;
        void ast(std::ostream& os) const noexcept final;
        [[nodiscard]] std::unique_ptr<expression> eval() const noexcept final;       
    };

    /*struct call_expr : public expression {
    private:
        std::unique_ptr<expression> m_callee;
        std::vector<std::unique_ptr<expression>> m_arguments;

        void pseudo(std::ostream &os) const noexcept final;
        void ast(std::ostream &os) const noexcept final;
    };*/

    struct grouping : public expression {
        grouping(std::unique_ptr<expression> expr) : m_expr(std::move(expr)) {};
        void pseudo(std::ostream& os) const noexcept final;
        void ast(std::ostream& os) const noexcept final;
        [[nodiscard]] std::unique_ptr<expression> eval() const noexcept final;
        [[nodiscard]] b8 operator==(const expression &rhs) const noexcept final;
    private:
        mutable std::unique_ptr<expression> m_expr;
    };

    

    struct identifier : public expression {
        identifier(const std::string &name, const u8 idx) : m_name(name), m_idx(idx) {};
        identifier(const u8 idx) : m_name("var_" + std::to_string(idx)), m_idx(idx) {};
        void pseudo(std::ostream& os) const noexcept final;
        void ast(std::ostream& os) const noexcept final;
        [[nodiscard]] std::unique_ptr<expression> eval() const noexcept final;
        [[nodiscard]] b8 operator==(const expression &rhs) const noexcept override;


    private:
        std::string m_name;
        u8 m_idx;        
    };
};