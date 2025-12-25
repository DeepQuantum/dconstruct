#include "ast/statements/variable_declaration.h"
#include "ast/statements/for.h"


namespace dconstruct::ast {

std::tuple<const std::string&, const expression&> for_stmt::get_for_each_iterable() const noexcept {
    assert(dynamic_cast<ast::compare_expr*>(m_condition.get()));
    const auto& comp = static_cast<ast::compare_expr&>(*m_condition);
    assert(dynamic_cast<ast::call_expr*>(comp.m_rhs.get()));
    const auto& call = static_cast<ast::call_expr&>(*comp.m_rhs);
    assert(dynamic_cast<ast::literal*>(call.m_callee.get()));
    const auto& callee = static_cast<ast::literal&>(*call.m_callee);
    assert(std::holds_alternative<sid64_literal>(callee.m_value));
    assert(std::get<sid64_literal>(callee.m_value).first == SID("darray-count"));
    
    assert(!call.m_arguments.empty());

    const auto& iterable = *call.m_arguments[0];


    assert(dynamic_cast<ast::block*>(m_body.get()));
    auto& block = static_cast<ast::block&>(*m_body);
    assert(!block.m_statements.empty());
    block.m_removedStatementsIndices.push_back(0);
    assert(dynamic_cast<ast::variable_declaration*>(block.m_statements[0].get()));
    assert(dynamic_cast<ast::variable_declaration*>(block.m_statements[0].get())->m_init.get());
    const auto& decl = static_cast<ast::variable_declaration&>(*block.m_statements[0]);
    const auto& init = dynamic_cast<ast::variable_declaration*>(block.m_statements[0].get())->m_init;
    assert(dynamic_cast<ast::call_expr*>(init.get()));
    const auto& at_call = static_cast<ast::call_expr&>(*init);
    assert(dynamic_cast<ast::literal*>(at_call.m_callee.get()));
    const auto& at_callee = static_cast<ast::literal&>(*at_call.m_callee);
    assert(std::get<sid64_literal>(at_callee.m_value).first == SID("darray-at"));
    assert(at_call.m_arguments.size() == 2);
    assert(*at_call.m_arguments[0] == iterable);

    return {decl.m_identifier, iterable};
}

void for_stmt::pseudo_c(std::ostream& os) const {
    if (!m_asForEach) {
        os << "for (" << *m_init << ' ' << *m_condition << "; " << *m_incr << ") " << *m_body;
    } else {
        const auto& [var_name, iterable] = get_for_each_iterable();
        os << "foreach (u64? " << var_name << " : " << iterable << ")" << *m_body;
    }
}


void for_stmt::pseudo_py(std::ostream& os) const {
    const auto& var = static_cast<const variable_declaration&>(*m_init).m_identifier;
    const auto& range = static_cast<const binary_expr&>(*m_condition).m_rhs;
    os << "for " << var << " in range(" << *range << "):\n" << *m_body;
}

void for_stmt::pseudo_racket(std::ostream& os) const {
    const auto& var = static_cast<variable_declaration&>(*m_init).m_identifier;
    const auto& range = static_cast<binary_expr&>(*m_condition).m_rhs;
    os << "(for ([" << var << " (in-range " << *range << ")])\n" << *m_body << ')';
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


VAR_OPTIMIZATION_ACTION for_stmt::var_optimization_pass(var_optimization_env& env)  noexcept {
    statement::check_var_optimization(&m_init, env);
    expression::check_var_optimization(&m_condition, env);
    expression::check_var_optimization(&m_incr, env);
    statement::check_var_optimization(&m_body, env);
    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION for_stmt::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    if (env) {
        m_asForEach = true;
    }
    statement::check_foreach_optimization(&m_init, env);
    if (auto action = m_condition->foreach_optimization_pass(env); action != FOREACH_OPTIMIZATION_ACTION::NONE) {
        return action;
    }
    if (auto action = m_incr->foreach_optimization_pass(env); action != FOREACH_OPTIMIZATION_ACTION::NONE) {
        return action;
    }
    statement::check_foreach_optimization(&m_body, env);
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

}