#include "ast/statements/variable_declaration.h"
#include "ast/primary_expressions/struct_access.h"

namespace dconstruct::ast {

    void variable_declaration::pseudo_c(ast_serialization_buffer& buffer) const {
        buffer.append(type_to_declaration_string(m_type), ' ', m_identifier);
        if (m_init != nullptr) {
            buffer.append(" = "sv, *m_init);
        }
        buffer.append(';');
    }

    void variable_declaration::to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept {
        type_to_colored_declaration_string(m_type, buffer);
        buffer.append(AST_COLOR::BLANK, ' ');
        const bool is_temp_var =
            (m_identifier.size() >= 5 && (m_identifier.compare(0, 4, "var_") == 0 || m_identifier.compare(0, 4, "arg_") == 0)) ||
            (m_identifier.size() == 1 && (m_identifier[0] == 'i' || m_identifier[0] == 'j' || m_identifier[0] == 'k' || m_identifier[0] == 'l'));
        buffer.append(is_temp_var ? AST_COLOR::IDENTIFIER : AST_COLOR::MEMBER, m_identifier);
        if (m_init != nullptr) {
            buffer.append(AST_COLOR::OPERATOR, " = "sv);
            buffer.append(*m_init);
        }
        buffer.append(AST_COLOR::PUNCTUATION, ';');
    }

    void variable_declaration::pseudo_py(ast_serialization_buffer& buffer) const {
        buffer.append(m_identifier);
        if (m_init != nullptr) {
            buffer.append(" = "sv, *m_init);
        }
    }

    void variable_declaration::pseudo_racket(ast_serialization_buffer& buffer) const {
        buffer.append("(define "sv, m_identifier);
        if (m_init != nullptr) {
            buffer.append(' ', *m_init);
        }
        buffer.append(')');
    }

    [[nodiscard]] bool variable_declaration::equals(const statement& rhs) const noexcept {
        const variable_declaration* rhs_ptr = dynamic_cast<const variable_declaration*>(&rhs);
        if (rhs_ptr == nullptr) {
            return false;
        }
        return m_type.index() == rhs_ptr->m_type.index() && m_identifier == rhs_ptr->m_identifier && m_init == rhs_ptr->m_init;
    }

    [[nodiscard]] std::unique_ptr<statement> variable_declaration::clone() const noexcept {
        return std::make_unique<variable_declaration>(m_type, m_identifier, m_init ? m_init->clone() : nullptr);
    }

    [[nodiscard]] std::vector<semantic_check_error> variable_declaration::check_semantics(compilation::scope& scope) const noexcept {
        if (m_init) {
            const semantic_check_res init_type = m_init->get_type_checked(scope);
            if (!init_type) {
                return {init_type.error()};
            }

            const std::optional<std::string> assign_err = not_assignable_reason(m_type, *init_type);

            if (assign_err) {
                return {semantic_check_error{*assign_err}};
            }
        }
        scope.define(m_identifier, m_type);

        return {};
    }

    [[nodiscard]] emission_err variable_declaration::emit_dc(
        compilation::function& fn,
        compilation::global_state& global
    ) const noexcept {
        const emission_res new_var_reg = fn.get_next_unused_register();
        if (!new_var_reg) {
            return new_var_reg.error();
        }
        assert(!fn.m_varsToRegs.lookup(m_identifier));
        fn.m_varsToRegs.define(m_identifier, *new_var_reg);

        if (m_init) {
            const emission_res init_emit = m_init->emit_dc(fn, global, *new_var_reg);
            if (!init_emit) {
                return init_emit.error();
            }
        }
        return std::nullopt;
    }

    VAR_OPTIMIZATION_ACTION variable_declaration::var_optimization_pass(var_optimization_env& env) noexcept {
        if (m_init) {
            env.check_action(&m_init);
        }
        if (!m_identifier.starts_with("var") || env.m_env.m_values.contains(m_identifier)) {
            return VAR_OPTIMIZATION_ACTION::NONE;
        }
        env.m_env.define(m_identifier, {});
        return VAR_OPTIMIZATION_ACTION::VAR_DECLARATION;
    }

    FOREACH_OPTIMIZATION_ACTION variable_declaration::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
        if (m_init) {
            return m_init->foreach_optimization_pass(env);
        }
        return FOREACH_OPTIMIZATION_ACTION::NONE;
    }

    MATCH_OPTIMIZATION_ACTION variable_declaration::match_optimization_pass(match_optimization_env& env) noexcept {
        return !m_init ? MATCH_OPTIMIZATION_ACTION::RESULT_VAR_DECLARATION : MATCH_OPTIMIZATION_ACTION::NONE;
    }

    void variable_declaration::member_access_optimization_pass() noexcept {
        if (!m_init) {
            return;
        }
        if (auto replacement = m_init->to_struct_access()) {
            m_init = std::move(replacement);
        }
    }

    void variable_declaration::regex_optimization_pass() noexcept {
        if (m_init) {
            m_init->regex_optimization_pass();
        }
    }

}
