#pragma once

#include "base.h"
#include "compilation/environment.h"


namespace dconstruct::ast {

    struct expression;
    struct statement;

    struct variable_folding_context {
        std::unique_ptr<statement>* m_declaration;
        std::vector<std::unique_ptr<expression>*> m_reads;
        std::vector<std::unique_ptr<expression>*> m_assigns;
    };

    struct var_optimization_env {
        compilation::environment<variable_folding_context> m_env;

        explicit var_optimization_env(compilation::environment<variable_folding_context>* enclosing = nullptr) noexcept
            : m_env(enclosing) {}

        void check_action(std::unique_ptr<expression>* expr);
        void check_action(std::unique_ptr<statement>* stmt);
    };

}