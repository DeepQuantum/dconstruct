#include "ast/statement.h"

namespace dconstruct::ast {
    struct while_stmt : public statement {
        explicit while_stmt(expr_uptr&& condition, stmnt_uptr&& body) noexcept :
        m_condition(std::move(condition)), m_body(std::move(body)) {};

        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
		void pseudo_racket(std::ostream& os) const final;

        [[nodiscard]] bool equals(const statement& rhs) const noexcept final;

        [[nodiscard]] std::unique_ptr<statement> clone() const noexcept final;

        VAR_FOLDING_ACTION decomp_optimization_pass(second_pass_env& env) noexcept;

        expr_uptr m_condition;
        stmnt_uptr m_body;
    };
}