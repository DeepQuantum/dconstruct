#include "ast/primary_expressions/enum_access.h"

namespace dconstruct::ast {

    [[nodiscard]] std::string enum_access::qualified_name() const {
        return std::format("{}.{}", m_enumName, m_memberName);
    }

    void enum_access::pseudo_c(ast_serialization_buffer& buffer) const {
        buffer.append(qualified_name());
    }

    void enum_access::pseudo_py(ast_serialization_buffer& buffer) const {
        buffer.append(qualified_name());
    }

    void enum_access::pseudo_racket(ast_serialization_buffer& buffer) const {
        buffer.append(qualified_name());
    }

    [[nodiscard]] expr_uptr enum_access::simplify() const {
        return clone();
    }

    [[nodiscard]] bool enum_access::equals(const expression& rhs) const noexcept {
        const enum_access* rhs_ptr = dynamic_cast<const enum_access*>(&rhs);
        if (rhs_ptr != nullptr) {
            return m_token == rhs_ptr->m_token && m_enumName == rhs_ptr->m_enumName && m_memberName == rhs_ptr->m_memberName && m_value == rhs_ptr->m_value;
        }
        return false;
    }

    [[nodiscard]] expr_uptr enum_access::clone() const {
        auto expr = std::make_unique<enum_access>(m_token, m_enumName, m_memberName, m_value);
        if (m_type)
            expr->set_type(*m_type);
        return expr;
    }

    [[nodiscard]] full_type enum_access::compute_type_unchecked(const compilation::scope& env) const noexcept {
        return m_value.compute_type_unchecked(env);
    }

    [[nodiscard]] u16 enum_access::calc_complexity() const noexcept {
        return m_value.get_complexity();
    }

    [[nodiscard]] const literal* enum_access::as_literal() const noexcept {
        return &m_value;
    }

    [[nodiscard]] semantic_check_res enum_access::compute_type_checked(compilation::scope& env) const noexcept {
        semantic_check_res type = m_value.compute_type_checked(env);
        if (!type) {
            return std::unexpected{semantic_check_error{type.error().m_message + " for enum member " + qualified_name(), this}};
        }
        return type;
    }

    [[nodiscard]] emission_res enum_access::emit_dc(
        compilation::function& fn,
        compilation::global_state& global,
        const std::optional<reg_idx> destination
    ) const noexcept {
        emission_res res = m_value.emit_dc(fn, global, destination);
        if (!res) {
            return std::unexpected{"can't compile enum member " + qualified_name() + ": " + res.error()};
        }
        return res;
    }

    VAR_OPTIMIZATION_ACTION enum_access::var_optimization_pass(var_optimization_env& env) noexcept {
        return VAR_OPTIMIZATION_ACTION::NONE;
    }

    FOREACH_OPTIMIZATION_ACTION enum_access::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
        return FOREACH_OPTIMIZATION_ACTION::NONE;
    }

    MATCH_OPTIMIZATION_ACTION enum_access::match_optimization_pass(match_optimization_env& env) noexcept {
        return MATCH_OPTIMIZATION_ACTION::LITERAL;
    }

}
