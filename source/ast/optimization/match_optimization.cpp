#include "ast/optimization/match_optimization.h"

namespace dconstruct::ast {

void match_optimization_env::check_action(expr_uptr* expr) {
    const auto pass_action = expr->get()->match_optimization_pass(*this);
    switch (pass_action) {
        case MATCH_OPTIMIZATION_ACTION::RESULT_VAR_ASSIGNMENT: {
            m_matches.push_back(expr);
            break;
        }
    }
}

void match_optimization_env::check_action(stmnt_uptr* statement) {
    const auto pass_action = statement->get()->match_optimization_pass(*this);
    switch (pass_action) {
        case MATCH_OPTIMIZATION_ACTION::RESULT_VAR_DECLARATION: {
            m_resultDeclaration = statement;
            break;
        }
    }
}

}