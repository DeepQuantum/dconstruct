#pragma once

#include "ast/statement.h"


namespace dconstruct::ast {

    struct var_optimization_env {
        compiler::environment<variable_folding_context> m_env;

        void check_action(std::unique_ptr<expression>* expr);
        void check_action(std::unique_ptr<statement>* stmt);
    };

}