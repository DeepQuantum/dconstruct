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
        unary_expr(std::unique_ptr<expression> lhs) : m_lhs(std::move(lhs)) {};

    protected:
        std::unique_ptr<expression> m_lhs;
    };

    struct logical_not_expr : public unary_expr {
        using unary_expr::unary_expr;
        
        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;
        [[nodiscard]] std::unique_ptr<expression> eval() const final;       
        [[nodiscard]] virtual b8 equals(const expression& other) const noexcept override;

    };

    struct call_expr : public expression {
        void pseudo(std::ostream &os) const final;
        void ast(std::ostream &os) const final;
    private:
        std::unique_ptr<expression> m_callee;
        std::vector<std::unique_ptr<expression>> m_arguments;
    };

    struct grouping : public expression {
        grouping(std::unique_ptr<expression> expr) : m_expr(std::move(expr)) {};
        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;
        [[nodiscard]] std::unique_ptr<expression> eval() const final;
        [[nodiscard]] b8 equals(const expression &rhs) const noexcept final;
    private:
        mutable std::unique_ptr<expression> m_expr;
    };

    struct identifier : public expression {
        identifier(const std::string &name, const u8 idx) : m_name(name), m_idx(idx) {};
        identifier(const u8 idx) : m_name("var_" + std::to_string(idx)), m_idx(idx) {};
        void pseudo(std::ostream& os) const final;
        void ast(std::ostream& os) const final;
        [[nodiscard]] std::unique_ptr<expression> eval() const final;
        [[nodiscard]] b8 equals(const expression &rhs) const noexcept override;


    private:
        std::string m_name;
        u8 m_idx;        
    };
};