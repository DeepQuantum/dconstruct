// #include "ast/expression.h"

// namespace dconstruct::ast {
//     struct binary_expr : public expression {
//     public:
//         binary_expr(compiler::token op, std::unique_ptr<expression>&& lhs, std::unique_ptr<expression>&& rhs) noexcept
//             : m_operator(std::move(op)), m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) 
//             {
//             }

//         inline void pseudo_c(std::ostream& os) const override {
//             os << *m_lhs << ' ' << m_operator.m_lexeme << ' ' << *m_rhs;
//         }

//         inline void pseudo_py(std::ostream& os) const override {
//             os << *m_lhs << ' ' << m_operator.m_lexeme << ' ' << *m_rhs;
//         }

//         inline void pseudo_racket(std::ostream& os) const override {
//             os << '(' << m_operator.m_lexeme << ' ' << *m_lhs << ' ' << *m_rhs << ')';
//         }

//         [[nodiscard]] inline full_type compute_type(const compiler::environment&) const override {
//             return full_type{ std::monostate() };
//         }

//         // for testing ! stupid and expensive.
//         [[nodiscard]] inline bool equals(const expression& rhs) const noexcept final {
//             const binary_expr* rhs_ptr = dynamic_cast<const binary_expr*>(&rhs);
//             if (rhs_ptr) {
//                 return typeid(*this) == typeid(*rhs_ptr) && m_rhs == rhs_ptr->m_rhs && m_lhs == rhs_ptr->m_lhs && m_operator == rhs_ptr->m_operator;
//             }
//             return false;
//         }

//         [[nodiscard]] inline u16 complexity() const noexcept override {
//             return 1 + m_lhs->complexity() + m_rhs->complexity();
//         }

//         compiler::token m_operator;
//         std::unique_ptr<expression> m_lhs;
//         std::unique_ptr<expression> m_rhs;
//     };
// }