#include "ast/statements/variable_declaration.h"
#include "ast/statements/foreach.h"
#include "ast/primary_expressions/struct_access.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/foreach_optimization.h"

#include "ast/statements/block.h"
#include "ctre.hpp"

namespace dconstruct::ast {

    void foreach_stmt::pseudo_c(ast_serialization_buffer& buffer) const {
        buffer.append("foreach ("sv, m_var, " : "sv, *m_iterable, ") "sv, *m_body);
    }

    void foreach_stmt::to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept {
        buffer.append(AST_COLOR::KEYWORD, "foreach "sv);
        buffer.append(AST_COLOR::PUNCTUATION, '(');
        buffer.append(m_var);
        buffer.append(AST_COLOR::PUNCTUATION, " : "sv);
        buffer.append(*m_iterable);
        buffer.append(AST_COLOR::PUNCTUATION, ") "sv);
        buffer.append(*m_body);
    }

    void foreach_stmt::pseudo_py(ast_serialization_buffer& buffer) const {
        buffer.append("for "sv, m_var, " in "sv, *m_iterable, ':', *m_body);
    }

    void foreach_stmt::pseudo_racket(ast_serialization_buffer& buffer) const {
        buffer.append("(for (["sv, m_var, ' ', *m_iterable, "]) "sv, *m_body, ')');
    }

    [[nodiscard]] bool foreach_stmt::equals(const statement& rhs) const noexcept {
        const foreach_stmt* rhs_ptr = dynamic_cast<const foreach_stmt*>(&rhs);
        if (rhs_ptr == nullptr) {
            return false;
        }
        return m_var == rhs_ptr->m_var && m_iterable == rhs_ptr->m_iterable && m_body == rhs_ptr->m_body;
    }

    [[nodiscard]] std::unique_ptr<statement> foreach_stmt::clone() const noexcept {
        return std::make_unique<foreach_stmt>(m_var, m_iterable->clone(), m_body->clone());
    }

    VAR_OPTIMIZATION_ACTION foreach_stmt::var_optimization_pass(var_optimization_env& env) noexcept {
        env.check_action(&m_iterable);
        env.check_action(&m_body);
        return VAR_OPTIMIZATION_ACTION::NONE;
    }

    FOREACH_OPTIMIZATION_ACTION foreach_stmt::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
        return FOREACH_OPTIMIZATION_ACTION::NONE;
    }

    MATCH_OPTIMIZATION_ACTION foreach_stmt::match_optimization_pass(match_optimization_env& env) noexcept {
        return m_body->match_optimization_pass(env);
    }

    void foreach_stmt::member_access_optimization_pass() noexcept {
        expression::replace_if_struct_access(m_iterable);
        m_body->member_access_optimization_pass();
    }

    void foreach_stmt::regex_optimization_pass() noexcept {
        static constexpr ctll::fixed_string pattern =
            R"(foreach \([a-zA-Z\d_]*\? var_\d+ : .*?\) \{)"
            R"(\s*u16 var_\d+;)"
            R"(\s*if \(var_\d+ && \*\(u16\*\)\(var_\d+ \+ 12\) == 7\) \{)"
            R"(\s*var_\d+ = \*\(u64\*\)var_\d+;\s*\})"
            R"(\s*else if \(var_\d+ && \*\(u16\*\)\(var_\d+ \+ 12\) == 5\) \{)"
            R"(\s*var_\d+ = \*\(u64\*\)var_\d+;\s*\})"
            R"(\s*else if \(var_\d+ && \*\(u16\*\)\(var_\d+ \+ 12\) == 4\) \{)"
            R"(\s*var_\d+ = \*\(u64\*\)var_\d+;\s*\})"
            R"(\s*else \{\s*var_\d+ = 0;\s*\})";

        m_iterable->regex_optimization_pass();
        m_body->regex_optimization_pass();

        std::string content = to_pseudo_c_string();

        if (ctre::starts_with<pattern>(content)) {
            block* body_block = static_cast<block*>(m_body.get());
            variable_declaration* decl_var = static_cast<variable_declaration*>(body_block->m_statements.begin()->get());

            m_var.m_type = std::move(decl_var->m_type);
            m_var.m_name = std::move(decl_var->m_identifier);

            body_block->m_statements.erase(body_block->m_statements.begin());
            body_block->m_statements.erase(std::next(body_block->m_statements.begin()));
        }
    }

}
