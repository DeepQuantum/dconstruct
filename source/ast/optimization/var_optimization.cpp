#include "ast/optimization/var_optimization.h"
#include "ast/assign.h"
#include "ast/statements/variable_declaration.h"


namespace dconstruct::ast {

void var_optimization_env::check_action(expr_uptr* expr) {
    const auto pass_action = expr->get()->var_optimization_pass(*this);
    switch (pass_action) {
        case VAR_OPTIMIZATION_ACTION::VAR_READ: {
            m_env.lookup(static_cast<identifier&>(**expr).m_name.m_lexeme)->m_reads.push_back(expr);
            break;
        } 
        case VAR_OPTIMIZATION_ACTION::VAR_WRITE: {
            auto* assign = static_cast<assign_expr*>(expr->get());
            assert(dynamic_cast<identifier*>(assign->m_lhs.get()));
            if (auto* var = m_env.lookup(static_cast<identifier&>(*assign->m_lhs).m_name.m_lexeme)) {
                var->m_assigns.push_back(expr);
            }
            break;
        }
        default: {
            break;
        }
    }
}

void var_optimization_env::check_action(stmnt_uptr* stmt) {
    const auto pass_action = stmt->get()->var_optimization_pass(*this);
    switch (pass_action) {
        case VAR_OPTIMIZATION_ACTION::VAR_DECLARATION: {
            auto& decl = static_cast<variable_declaration&>(**stmt);
            auto context = variable_folding_context{stmt, {}, {}};
            m_env.define(decl.m_identifier, std::move(context));
            break;
        }
        default: {
            break;
        }
    }
}
}