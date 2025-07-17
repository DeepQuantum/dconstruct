#pragma once

#include "base.h"
#include "expressions.h"
#include <vector>
#include <ostream>

namespace dconstruct::dcompiler {

    struct statement {
        virtual void pseudo(std::ostream&) const noexcept = 0;
        virtual void ast(std::ostream&) const noexcept = 0;
    };

    struct block_stmt : public statement {
        void pseudo(std::ostream& os) const noexcept override {
            os << '{';
            for (const auto& stmt : m_statements) {
                stmt->pseudo(os);
                os << '\n';
            }
            os << '}';
        }

        void ast(std::ostream& os) const noexcept override {
            os << "block[";
            for (const auto& stmt : m_statements) {
                stmt->ast(os);
                os << ',';
            }
            os << ']';
        }
        
    private:
        std::vector<statement*> m_statements;
    };

    struct if_stmt : public statement {
        
        void pseudo(std::ostream& os) const noexcept override {
            os << "if (";
            m_condition->pseudo(os);
            os << ") ";
            m_body->pseudo(os);
        }

        void ast(std::ostream& os) const noexcept override {
            os << "if[cond=";
            m_condition->pseudo(os);
            os << ", body=";
            m_body->pseudo(os);
            os << ']';
        }

    private:
        expression* m_condition;
        block_stmt* m_body;
    };
}