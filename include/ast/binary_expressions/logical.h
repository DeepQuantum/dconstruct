#include "ast/binary_expression.h"
#include "compilation/tokens.h"

namespace dconstruct::ast {
    struct logical_expr : public clonable_binary_expr<logical_expr> {
        using clonable_binary_expr::clonable_binary_expr;
        [[nodiscard]] expr_uptr simplify() const final;

        [[nodiscard]] semantic_check_res compute_type_checked(compilation::scope& env) const noexcept final;
        [[nodiscard]] resstr<reg_idx> emit_dc(compilation::function_context& fn, compilation::global_state& global, const std::optional<reg_idx> destination) const noexcept final;
        [[nodiscard]] resstr<std::vector<u64>> emit_dc_branch(compilation::function_context& fn, compilation::global_state& global, bool branch_when_true) const noexcept final;

        void pseudo_py(ast_serialization_buffer& buffer) const final;
        void pseudo_racket(ast_serialization_buffer& buffer) const final;
    };
}
