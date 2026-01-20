#include "ast/statements/if.h"
#include "ast/statements/block.h"
#include "ast/assign.h"
#include <iostream>


namespace dconstruct::ast {

void if_stmt::pseudo_c(std::ostream& os) const {
    os << "if (" << *m_condition << ") " << *m_then;
    if (m_else && !m_else->is_dead_code()) {
        if (const auto* statement = m_else->inlineable_else_statement()) {
        os << " else " << *statement;
        } else {
            os << " else " << *m_else;
        }
    }
}

void if_stmt::pseudo_py(std::ostream& os) const {
    os << "if " << *m_condition << ' ' << *m_then;
    if (m_else != nullptr) {
        os << " else " << *m_else;
    }
}

void if_stmt::pseudo_racket(std::ostream& os) const {
    os << "(if "<< *m_condition << " " << *m_then << " ";
    if (m_else != nullptr) {
        os << *m_else;
    } else {
        os << "(void)";
    }
    os << ")";
}

[[nodiscard]] bool if_stmt::equals(const statement& rhs) const noexcept {
    const if_stmt* rhs_ptr = dynamic_cast<const if_stmt*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_condition == rhs_ptr->m_condition && m_then == rhs_ptr->m_then && m_else ? (m_else == rhs_ptr->m_else) : !rhs_ptr->m_else;
}

[[nodiscard]] std::unique_ptr<statement> if_stmt::clone() const noexcept {
    return std::make_unique<if_stmt>(m_condition->clone(), m_then->clone(), m_else ? m_else->clone() : nullptr);
}


[[nodiscard]] const statement* if_stmt::inlineable_else_statement() const noexcept {
    return this;
}

[[nodiscard]] std::vector<semantic_check_error> if_stmt::check_semantics(compiler::scope& env) const noexcept {
    std::vector<semantic_check_error> errors;
    
    semantic_check_res cond_type = m_condition->get_type_checked(env);

    if (!cond_type) {
        errors.push_back(std::move(cond_type.error()));
    }

    std::optional<std::string> invalid_condition = std::visit([](auto&& cond) -> std::optional<std::string> {
        using cond_t = std::decay_t<decltype(cond)>;

        if constexpr (is_primitive<cond_t>) {
            if (is_arithmetic(cond.m_type)) {
                return std::nullopt;
            }
            return "if condition must be of arithmetic type, but got " + type_to_declaration_string(cond);
        }
        return "if condition must be of arithmetic type, but got " + type_to_declaration_string(cond);

    }, *cond_type);

    if (invalid_condition) {
        errors.push_back(semantic_check_error{std::move(*invalid_condition), m_condition.get()});
    } else {
        std::vector<semantic_check_error> then_errors = m_then->check_semantics(env);
        if (!then_errors.empty()) {
            errors.insert(errors.end(), then_errors.begin(), then_errors.end());
        }
        if (m_else) {
            std::vector<semantic_check_error> else_errors = m_else->check_semantics(env);
            if (!else_errors.empty()) {
                errors.insert(errors.end(), else_errors.begin(), else_errors.end());
            }
        }
    }

    return errors;
}


VAR_OPTIMIZATION_ACTION if_stmt::var_optimization_pass(var_optimization_env& env)  noexcept {
    env.check_action(&m_condition);
    env.check_action(&m_then);
    if (m_else) {
        env.check_action(&m_else);
    }
    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION if_stmt::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    if (const auto action = m_condition->foreach_optimization_pass(env); action != FOREACH_OPTIMIZATION_ACTION::NONE) {
        return action;
    }
    env.check_action(&m_then);
    if (m_else) {
        env.check_action(&m_else);
    }
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

MATCH_OPTIMIZATION_ACTION if_stmt::match_optimization_pass(match_optimization_env& env) noexcept {
    if (m_condition->match_optimization_pass(env) == MATCH_OPTIMIZATION_ACTION::MATCH_CONDITION_COMPARISON) {
        if (m_then->match_optimization_pass(env) == MATCH_OPTIMIZATION_ACTION::RESULT_VAR_ASSIGNMENT) {
            if (m_else && m_else->match_optimization_pass(env) == MATCH_OPTIMIZATION_ACTION::RESULT_VAR_ASSIGNMENT) {
                return MATCH_OPTIMIZATION_ACTION::RESULT_VAR_ASSIGNMENT;
            }
        }
    }
    return MATCH_OPTIMIZATION_ACTION::NONE;
}
}