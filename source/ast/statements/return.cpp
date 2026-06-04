#include "ast/statements/return.h"
#include "ast/primary_expressions/struct_access.h"

namespace dconstruct::ast {

    void return_stmt::pseudo_c(ast_serialization_buffer& buffer) const {
        if (m_expr) {
            buffer.append("return "sv, *m_expr, ';');
        } else {
            buffer.append("return;"sv);
        }
    }

    void return_stmt::to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept {
        if (m_expr) {
            buffer.append(AST_COLOR::KEYWORD, "return "sv);
            buffer.append(*m_expr);
            buffer.append(AST_COLOR::PUNCTUATION, ';');
        } else {
            buffer.append(AST_COLOR::KEYWORD, "return"sv);
            buffer.append(AST_COLOR::PUNCTUATION, ';');
        }
    }

    void return_stmt::pseudo_py(ast_serialization_buffer& buffer) const {
        if (m_expr) {
            buffer.append("return "sv, *m_expr);
        } else {
            buffer.append("return;"sv);
        }
    }

    void return_stmt::pseudo_racket(ast_serialization_buffer& buffer) const {
        if (m_expr) {
            buffer.append(*m_expr);
        }
    }

    [[nodiscard]] bool return_stmt::equals(const statement& rhs) const noexcept {
        const return_stmt* rhs_ptr = dynamic_cast<const return_stmt*>(&rhs);
        if (rhs_ptr == nullptr) {
            return false;
        }
        return m_expr == rhs_ptr->m_expr;
    }

    [[nodiscard]] std::unique_ptr<statement> return_stmt::clone() const noexcept {
        return std::make_unique<return_stmt>(m_expr->clone());
    }

    [[nodiscard]] std::vector<semantic_check_error> return_stmt::check_semantics(compilation::scope& scope) const noexcept {
        if (m_expr) {
            const semantic_check_res expr_type = m_expr->get_type_checked(scope);
            if (!expr_type) {
                return {expr_type.error()};
            }
            if (const std::optional<std::string> err = not_assignable_reason(*scope.m_expectedReturnType, *expr_type)) {
                return {semantic_check_error{*err}};
            }
            scope.m_computedReturnType = true;
            return {};
        } else {
            if (!std::holds_alternative<std::monostate>(*scope.m_expectedReturnType)) {
                return {semantic_check_error{"cannot return a value because function has type void", this}};
            }
            scope.m_computedReturnType = true;
            return {};
        }
    }

    [[nodiscard]] emission_err return_stmt::emit_dc(
        compilation::function& fn,
        compilation::global_state& global
    ) const noexcept {
        const emission_res expr_res = m_expr->emit_dc(fn, global);
        if (!expr_res) {
            return expr_res.error();
        }
        if (*expr_res != 0_r) {
            fn.m_returnBranchLocations.push_back(fn.m_instructions.size());
            fn.emit_instruction(Opcode::Move, 0_r, *expr_res);
        } else {
            fn.m_returnBranchLocations.push_back(fn.m_instructions.size() - 1);
        }
        fn.emit_instruction(Opcode::Branch, compilation::function::BRANCH_PLACEHOLDER, 0_r, compilation::function::BRANCH_PLACEHOLDER);
        return std::nullopt;
    }

    VAR_OPTIMIZATION_ACTION return_stmt::var_optimization_pass(var_optimization_env& env) noexcept {
        env.check_action(&m_expr);
        return VAR_OPTIMIZATION_ACTION::NONE;
    }

    FOREACH_OPTIMIZATION_ACTION return_stmt::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
        return m_expr->foreach_optimization_pass(env);
    }

    MATCH_OPTIMIZATION_ACTION return_stmt::match_optimization_pass(match_optimization_env& env) noexcept {
        return MATCH_OPTIMIZATION_ACTION::NONE;
    }

    void return_stmt::member_access_optimization_pass() noexcept {
        expression::replace_if_struct_access(m_expr);
    }

    void return_stmt::regex_optimization_pass() noexcept {
        if (m_expr) {
            m_expr->regex_optimization_pass();
        }
    }

}
