#include "ast/statements/return.h"

namespace dconstruct::ast {

void return_stmt::pseudo_c(std::ostream& os) const {
    if (m_expr) {
        os << "return " << *m_expr << ';';
    } else {
        os << "return;";
    }
}

void return_stmt::pseudo_py(std::ostream& os) const {
    if (m_expr) {
        os << "return " << *m_expr << '\n';
    } else {
        os << "return;\n";
    }
}

void return_stmt::pseudo_racket(std::ostream& os) const {
    return;
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
        if (const std::optional<std::string> err = is_assignable(*scope.m_returnType, *expr_type)) {
            return {semantic_check_error{*err}};
        }
        return {};
    } else {
        if (!std::holds_alternative<std::monostate>(*scope.m_returnType)) {
            return {semantic_check_error{"cannot return a value because function has type void", this}};
        }
        return {};
    }
}

[[nodiscard]] emission_err return_stmt::emit_dc(compilation::function& fn, compilation::global_state& global) const noexcept {
    const emission_res expr_res = m_expr->emit_dc(fn, global);
    if (!expr_res) {
        return expr_res.error();
    }
    fn.emit_instruction(Opcode::Return, *expr_res, *expr_res);
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

}