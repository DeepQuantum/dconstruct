#include "base.h"
#include "expressions.h"
#include <vector>
#include <ostream>

namespace dconstruct::dcompiler {

    struct statement {
        virtual std::ostream& pseudo(std::ostream&) const noexcept = 0;
        virtual std::ostream& ast(std::ostream&) const noexcept = 0;
    };

    struct block_stmt : public statement {
        std::ostream& pseudo(std::ostream& os) const noexcept override {
            os << '{';
            for (const auto& stmt : m_statements) {
                stmt->pseudo(os);
                os << '\n';
            }
            return os << '}';
        }

        std::ostream& ast(std::ostream& os) const noexcept override {
            os << "block[";
            for (const auto& stmt : m_statements) {
                stmt->ast(os);
                os << ',';
            }
            return os << ']';
        }
        
    private:
        std::vector<statement*> m_statements;
    };

    struct if_stmt : public statement {
        
        std::ostream& pseudo(std::ostream& os) const noexcept override {
            return os << "if (";
            m_condition->pseudo(os) << ") ";
            m_body->pseudo(os);
        }

        std::ostream& ast(std::ostream& os) const noexcept override {
            return os << "if[cond=";
            m_condition->pseudo(os) << ", body=";
            m_body->pseudo(os);
            os << ']';
        }

    private:
        expression* m_condition;
        block_stmt* m_body;
    };
}