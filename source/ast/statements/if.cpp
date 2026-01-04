#include "ast/statements/if.h"
#include "ast/statements/block.h"
#include "ast/assign.h"

namespace dconstruct::ast {

void if_stmt::pseudo_c(std::ostream& os) const {
    os << "if (" << *m_condition << ") " << *m_then;
    if (m_else) {
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
    return m_condition == rhs_ptr->m_condition && m_then == rhs_ptr->m_then && m_else == rhs_ptr->m_then;
}

[[nodiscard]] std::unique_ptr<statement> if_stmt::clone() const noexcept {
    return std::make_unique<if_stmt>(m_condition->clone(), m_then->clone(), m_else ? m_else->clone() : nullptr);
}


VAR_OPTIMIZATION_ACTION if_stmt::var_optimization_pass(var_optimization_env& env)  noexcept {
    expression::check_var_optimization(&m_condition, env);
    statement::check_var_optimization(&m_then, env);
    if (m_else) {
        statement::check_var_optimization(&m_else, env);
    }
    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION if_stmt::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    if (const auto action = m_condition->foreach_optimization_pass(env); action != FOREACH_OPTIMIZATION_ACTION::NONE) {
        return action;
    }
    statement::check_foreach_optimization(&m_then, env);
    if (m_else) {
        statement::check_foreach_optimization(&m_else, env);
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