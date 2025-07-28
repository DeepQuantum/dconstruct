#pragma once

#include "base.h"
#include "expressions.h"
#include "binary_expressions.h"
#include <vector>
#include "tokens.h"

namespace dconstruct::ast {

    struct statement {
        virtual ~statement() = default;
        virtual void pseudo(std::ostream&) const = 0;
        virtual void ast(std::ostream&) const = 0;
        [[nodiscard]] virtual b8 equals(const statement &rhs) const noexcept = 0;
    };

    [[nodiscard]] inline b8 operator==(const statement& lhs, const statement& rhs) noexcept {
        return lhs.equals(rhs);
    }

    [[nodiscard]] inline b8 operator==(const std::unique_ptr<statement>& lhs, const std::unique_ptr<statement>& rhs) noexcept {
        return *lhs == *rhs;
    }

    inline std::ostream& operator<<(std::ostream& os, const statement &expr) {
        os << set_ast;
        if (os.iword(get_flag_index()) & AST) {
            expr.ast(os);
        } else {
            expr.pseudo(os);
        }
        return os;
    }

    

    // struct block_stmt : public statement {
    //     void pseudo(pseudo_stream& os) const noexcept override {
    //         os << '{';
    //         for (const auto& stmt : m_statements) {
    //             stmt->pseudo(os);
    //             os << '\n';
    //         }
    //         os << '}';
    //     }

    //     void ast(ast_stream& os) const noexcept override {
    //         os << "block[";
    //         for (const auto& stmt : m_statements) {
    //             stmt->ast(os);
    //             os << ',';
    //         }
    //         os << ']';
    //     }
        
    // private:
    //     std::vector<const statement*> m_statements;
    // };

    // struct if_stmt : public statement {
        
    //     void pseudo(pseudo_stream& os) const noexcept override {
    //         os << "if (";
    //         m_condition->pseudo(os);
    //         os << ") ";
    //         m_body->pseudo(os);
    //     }

    //     void ast(ast_stream& os) const noexcept override {
    //         os << "if[cond=";
    //         m_condition->pseudo(os);
    //         os << ", body=";
    //         m_body->pseudo(os);
    //         os << ']';
    //     }

    // private:
    //     const expression* m_condition;
    //     const block_stmt* m_body;
    // };
}