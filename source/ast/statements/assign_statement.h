#pragma once

#include "statements.h" 
#include "type.h"

namespace dconstruct::ast {

struct assign_statement : public statement {
    
    assign_statement(const assign_expr* expr) : m_type(E_UNKNOWN), m_expr(expr) {};
    assign_statement(const type type, const assign_expr* expr) : m_type(type), m_expr(expr) {};
    void pseudo(std::ostream&) const final;
    void ast(std::ostream&) const final;
    [[nodiscard]] virtual b8 equals(const statement &rhs) const noexcept override;

    [[nodiscard]] const assign_expr* get_expr() const noexcept { return m_expr; }
private:
    const type m_type;
    const assign_expr* m_expr;
};

}