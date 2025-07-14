#include "base.h"
#include <vector>
#include <ostream>

namespace dconstruct::dcompiler {

    struct expression {
    public:
        virtual std::ostream& pseudo(std::ostream&) const noexcept = 0;
        virtual std::ostream& ast(std::ostream&) const noexcept = 0;
    };

    struct binary_expr: public expression {
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

        std::ostream& pseudo(std::ostream& os) const noexcept override {
            return m_lhs->pseudo(os) << " + ";
            m_rhs->pseudo(os);
        }

        std::ostream& ast(std::ostream& os) const noexcept override {
            os << "add[";
            m_lhs->ast(os) << ", ";
            m_rhs->ast(os) << ']';
        }
    };

    struct call_expr : public expression {

    private:
        const expression* m_callee;
        std::vector<const expression*> m_arguments;

        std::ostream& pseudo(std::ostream &os) const noexcept override {
            m_callee->pseudo(os) << "(";
            for (const auto& arg : m_arguments) {
                arg->pseudo(os);
                os << ',';
            }
            return os << ')';
        }

        std::ostream& ast(std::ostream &os) const noexcept override {
            os << "Call[callee=";
            m_callee->ast(os);
            os << ", arguments={";
            for (const auto& arg : m_arguments) {
                arg->ast(os);
                os << ',';
            }
            return os << "}]";
        }
    };

    

    struct literal : public unary_expr {
    public:
        std::ostream& ast(std::ostream& os) const noexcept override {
            os << std::to_string(m_lhs.num);
        };    
    
    private:
        union literal_type {
            std::string str;
            u64 num;
        } m_lhs;

    };


};