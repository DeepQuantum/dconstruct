#include "ast/statements/expression_statement.h"
#include "ast/primary_expressions/struct_access.h"

namespace dconstruct::ast {

    void expression_stmt::pseudo_c(ast_serialization_buffer& buffer) const {
        buffer.append(*m_expression, ';');
    }

    void expression_stmt::to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept {
        buffer.append(*m_expression);
        buffer.append(AST_COLOR::PUNCTUATION, ';');
    }

    void expression_stmt::pseudo_py(ast_serialization_buffer& buffer) const {
        buffer.append(*m_expression);
    }

    void expression_stmt::pseudo_racket(ast_serialization_buffer& buffer) const {
        buffer.append(*m_expression);
    }

    [[nodiscard]] bool expression_stmt::equals(const statement& rhs) const noexcept {
        const expression_stmt* rhs_ptr = dynamic_cast<const expression_stmt*>(&rhs);
        if (rhs_ptr == nullptr) {
            return false;
        }
        return *m_expression == *rhs_ptr->m_expression;
    }

    [[nodisarc]] std::unique_ptr<statement> expression_stmt::clone() const noexcept {
        return std::make_unique<expression_stmt>(m_expression->clone());
    }

    [[nodiscard]] std::vector<semantic_check_error> expression_stmt::check_semantics(compilation::scope& env) const noexcept {
        semantic_check_res expr_type = m_expression->get_type_checked(env);

        if (!expr_type) {
            std::vector<semantic_check_error> res;
            res.push_back(std::move(expr_type.error()));
            return res;
        }

        return {};
    }

    [[nodiscard]] errmsg expression_stmt::emit_dc(
        compilation::function_context& fn,
        compilation::global_state& global
    ) const noexcept {
        const resstr<reg_idx> expr_res = m_expression->emit_dc(fn, global);
        if (!expr_res) {
            return expr_res.error();
        }
        fn.free_register(*expr_res);
        return std::nullopt;
    }

    VAR_OPTIMIZATION_ACTION expression_stmt::var_optimization_pass(var_optimization_env& env) noexcept {
        env.check_action(&m_expression);
        return VAR_OPTIMIZATION_ACTION::NONE;
    }

    FOREACH_OPTIMIZATION_ACTION expression_stmt::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
        return m_expression->foreach_optimization_pass(env);
    }

    MATCH_OPTIMIZATION_ACTION expression_stmt::match_optimization_pass(match_optimization_env& env) noexcept {
        return m_expression->match_optimization_pass(env);
    }

    void expression_stmt::member_access_optimization_pass() noexcept {
        expression::replace_if_struct_access(m_expression);
    }

    void expression_stmt::regex_optimization_pass() noexcept {
        m_expression->regex_optimization_pass();
    }

    [[nodiscard]] bool expression_stmt::is_dead_code() const noexcept {
        return m_expression->is_dead_code();
    }

}
