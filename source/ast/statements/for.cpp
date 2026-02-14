#include "ast/statements/variable_declaration.h"
#include "ast/statements/for.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/foreach_optimization.h"

namespace dconstruct::ast {

void for_stmt::pseudo_c(std::ostream& os) const {
    os << "for (" << *m_init << ' ' << *m_condition << "; " << *m_incr << ") " << *m_body;
}


void for_stmt::pseudo_py(std::ostream& os) const {
    const auto& var = static_cast<const variable_declaration&>(*m_init).m_identifier;
    const auto& range = static_cast<const binary_expr&>(*m_condition).m_rhs;
    os << "for " << var << " in range(" << *range << "):";
    os << *m_body;
}

void for_stmt::pseudo_racket(std::ostream& os) const {
    const auto& var = static_cast<const variable_declaration&>(*m_init).m_identifier;
    const auto& range = static_cast<const binary_expr&>(*m_condition).m_rhs;
    os << "(for ([ " << var << " (in-range " << *range << ")]) " << *m_body << ")";
}

[[nodiscard]] bool for_stmt::equals(const statement& rhs) const noexcept {
    const for_stmt* rhs_ptr = dynamic_cast<const for_stmt*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_init == rhs_ptr->m_init && m_condition == rhs_ptr->m_condition && m_incr == rhs_ptr->m_incr && m_body == rhs_ptr->m_body;
}

[[nodiscard]] std::unique_ptr<statement> for_stmt::clone() const noexcept {
    return std::make_unique<for_stmt>(m_init->clone(), m_condition->clone(), m_incr->clone(), m_body->clone());
}

[[nodiscard]] std::vector<semantic_check_error> for_stmt::check_semantics(compilation::scope& scope) const noexcept {
    std::vector<semantic_check_error> errors;

    compilation::scope nested{&scope};
    
    std::vector<semantic_check_error> init_errors = m_init->check_semantics(nested);
    if (!init_errors.empty()) {
        errors = std::move(init_errors);
    }

    semantic_check_res cond_type = m_condition->get_type_checked(nested);
    if (!cond_type) {
        errors.emplace_back(std::move(cond_type.error()));
    }

    std::optional<std::string> invalid_condition = std::visit([](auto&& cond) -> std::optional<std::string> {
        using cond_t = std::decay_t<decltype(cond)>;

        if constexpr (is_primitive<cond_t>) {
            if (is_arithmetic(cond.m_type)) {
                return std::nullopt;
            }
            return "for loop condition must be of arithmetic type, but got " + type_to_declaration_string(cond);
        }
        return "for loop condition must be of arithmetic type, but got " + type_to_declaration_string(cond);

    }, *cond_type);

    if (invalid_condition) {
        errors.emplace_back(std::move(*invalid_condition), m_condition.get());
    }
    
    semantic_check_res incr_type = m_incr->get_type_checked(nested);
    if (!incr_type) {
        errors.emplace_back(std::move(incr_type.error()));
    }

    std::vector<semantic_check_error> body_errors = m_body->check_semantics(nested);
    if (!body_errors.empty()) {
        errors.insert(errors.end(), body_errors.begin(), body_errors.end());
    }

    return errors;
}

[[nodiscard]] emission_err for_stmt::emit_dc(compilation::function& fn, compilation::global_state& global) const noexcept {
    constexpr u8 BRANCH_PLACEHOLDER = 0xFF;

    compilation::environment<reg_idx> old_saved = std::move(fn.m_varsToRegs);
    compilation::environment<reg_idx> new_env(&old_saved);
    fn.m_varsToRegs = std::move(new_env);

    // init
    // condition
    // body
    // incr

    const emission_err init_err = m_init->emit_dc(fn, global);
    if (init_err) {
        return init_err;
    }

    u16 head_location = fn.m_instructions.size();

    const emission_res condition_reg = m_condition->emit_dc(fn, global);
    if (!condition_reg) {
        return condition_reg.error();
    }

    const u16 branch_location = fn.m_instructions.size();
    fn.emit_instruction(Opcode::BranchIfNot, BRANCH_PLACEHOLDER, *condition_reg, BRANCH_PLACEHOLDER);

    const emission_err body_err = m_body->emit_dc(fn, global);
    if (body_err) {
        return body_err;
    }

    const emission_res incr_res = m_incr->emit_dc(fn, global);
    if (!incr_res) {
        return incr_res.error();
    }

    const u16 body_end = fn.m_instructions.size() + 1;
    fn.m_instructions[branch_location].set_lo_hi(body_end);

    const u8 start_branch_lo = head_location & 0xFF;
    const u8 start_branch_hi = (head_location >> 8) & 0xFF; 

    fn.emit_instruction(Opcode::Branch, start_branch_lo, 00, start_branch_hi);
    fn.free_register(*condition_reg);

    for (const auto& [_, reg] : fn.m_varsToRegs.m_values) {
        fn.free_lvalue_register(reg);
    }

    fn.m_varsToRegs = std::move(old_saved);

    return std::nullopt;
}


VAR_OPTIMIZATION_ACTION for_stmt::var_optimization_pass(var_optimization_env& env)  noexcept {
    env.check_action(&m_init);
    env.check_action(&m_condition);
    env.check_action(&m_incr);
    env.check_action(&m_body);

    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION for_stmt::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    env.check_action(&m_condition);
    env.check_action(&m_body);
    return FOREACH_OPTIMIZATION_ACTION::FOR;
}

MATCH_OPTIMIZATION_ACTION for_stmt::match_optimization_pass(match_optimization_env& env) noexcept {
    return m_body->match_optimization_pass(env);
}

}