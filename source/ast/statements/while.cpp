#include "ast/statements/while.h"


namespace dconstruct::ast {

void while_stmt::pseudo_c(std::ostream& os) const {
    os << "while (" << *m_condition << ") " << *m_body;
}


void while_stmt::pseudo_py(std::ostream& os) const {
    os << "while " << *m_condition << ":\n" << *m_body;
}

void while_stmt::pseudo_racket(std::ostream& os) const {
    os << "(while " << *m_condition << " " << *m_body << ")";
}



[[nodiscard]] bool while_stmt::equals(const statement& rhs) const noexcept {
    const while_stmt* rhs_ptr = dynamic_cast<const while_stmt*>(&rhs);
    if (rhs_ptr == nullptr) {
        return false;
    }
    return m_condition == rhs_ptr->m_condition && m_body == rhs_ptr->m_body;
}


[[nodiscard]] std::unique_ptr<statement> while_stmt::clone() const noexcept {
    return std::make_unique<while_stmt>(m_condition->clone(), m_body->clone());
}

[[nodiscard]] std::vector<semantic_check_error> while_stmt::check_semantics(type_environment& env) const noexcept {
    std::vector<semantic_check_error> errors;
    
    semantic_check_res cond_type = m_condition->get_type_checked(env);

    if (!cond_type) {
        errors.push_back(std::move(cond_type.error()));
    }

    std::optional<std::string> invalid_condition = std::visit([](auto&& cond) -> std::optional<std::string> {
        using cond_t = std::decay_t<decltype(cond)>;

        if constexpr (is_primitive<cond_t>) {
            if constexpr (is_arithmethic(cond)) {
                return std::nullopt;
            }
            return "while loop condition must be of arithmetic type, but got " + type_to_declaration_string(cond);
        }
        return "while loop condition must be of arithmetic type, but got " + type_to_declaration_string(cond);

    }, *cond_type);

    if (invalid_condition) {
        errors.emplace_back(std::move(*invalid_condition), m_condition.get());
    } else {
        std::vector<semantic_check_error> body_errors = m_body->check_semantics(env);
        if (!body_errors.empty()) {
            errors.insert(errors.end(), body_errors.begin(), body_errors.end());
        }
    }

    return errors;
}


VAR_OPTIMIZATION_ACTION while_stmt::var_optimization_pass(var_optimization_env& env)  noexcept {
    env.check_action(&m_condition);
    env.check_action(&m_body);
    return VAR_OPTIMIZATION_ACTION::NONE;
}


FOREACH_OPTIMIZATION_ACTION while_stmt::foreach_optimization_pass(foreach_optimization_env& env)  noexcept {
    if (auto action = m_condition->foreach_optimization_pass(env); action != FOREACH_OPTIMIZATION_ACTION::NONE) {
        return action;
    }
    env.check_action(&m_body);
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

MATCH_OPTIMIZATION_ACTION while_stmt::match_optimization_pass(match_optimization_env& env) noexcept {
    return m_body->match_optimization_pass(env);
}

}