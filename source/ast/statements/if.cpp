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

[[nodiscard]] std::vector<semantic_check_error> if_stmt::check_semantics(compilation::scope& env) const noexcept {
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

[[nodiscard]] emission_err if_stmt::emit_dc(compilation::function& fn, compilation::global_state& global) const noexcept {

    const emission_res condition = m_condition->emit_dc(fn, global);
    if (!condition) {
        return condition.error();
    }

    const u64 conditional_branch_location = fn.m_instructions.size();
    fn.emit_instruction(Opcode::BranchIfNot, compilation::function::BRANCH_PLACEHOLDER, *condition, compilation::function::BRANCH_PLACEHOLDER);

    const emission_err then_err = m_then->emit_dc(fn, global);
    if (then_err) {
        return then_err;
    }

    u64 else_skip_location = 0;
    if (m_else) {
        else_skip_location = fn.m_instructions.size();
        fn.emit_instruction(Opcode::Branch, compilation::function::BRANCH_PLACEHOLDER, 00, compilation::function::BRANCH_PLACEHOLDER);

        const emission_err else_err = m_else->emit_dc(fn, global);
        if (else_err) {
            return else_err;
        }
    }

    const u64 end_location = fn.m_instructions.size();
    if (else_skip_location == 0) {
        fn.m_instructions[conditional_branch_location].set_lo_hi(end_location);
    } else {
        fn.m_instructions[conditional_branch_location].set_lo_hi(else_skip_location + 1);
        fn.m_instructions[else_skip_location].set_lo_hi(end_location);
    }

    return std::nullopt;
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