#include "ast/optimization/foreach_optimization.h"

namespace dconstruct::ast {

void foreach_optimization_env::check_action(expr_uptr* expr) {
    const auto pass_action = expr->get()->foreach_optimization_pass(*this);
    switch (pass_action) {
        case FOREACH_OPTIMIZATION_ACTION::DARRAY_AT: {
            m_darrayAt.emplace_back(expr);
            break;
        }
        default: {
            break;
        }
    }
}

void foreach_optimization_env::check_action(stmnt_uptr* stmt) {
    const auto pass_action = stmt->get()->foreach_optimization_pass(*this);
    switch (pass_action) {
        case FOREACH_OPTIMIZATION_ACTION::BEGIN_FOREACH: {
            m_beginForeach.push_back(stmt);
            break;
        }
        case FOREACH_OPTIMIZATION_ACTION::END_FOREACH: {
            m_endForeach.push_back(stmt);
            break;
        }
        case FOREACH_OPTIMIZATION_ACTION::DARRAY_AT: {
            m_darrayAt.emplace_back(stmt);
            break;
        }
        case FOREACH_OPTIMIZATION_ACTION::FOR: {
            m_for.push_back(stmt);
            break;
        }
        default: {
            break;
        }
    }
}
}