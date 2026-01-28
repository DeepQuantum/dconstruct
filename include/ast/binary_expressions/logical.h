#include "ast/binary_expression.h"
#include "compilation/tokens.h"


namespace dconstruct::ast {
    struct logical_expr : public clonable_binary_expr<logical_expr> {
        using clonable_binary_expr::clonable_binary_expr;
        [[nodiscard]] expr_uptr simplify() const final;

        [[nodiscard]] semantic_check_res compute_type_checked(compilation::scope& env) const noexcept final;

        /*void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
		void pseudo_racket(std::ostream& os) const final;*/
    };
}