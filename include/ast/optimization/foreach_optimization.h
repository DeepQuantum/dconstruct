#pragma once
#include "ast/statement.h"


namespace dconstruct::ast {

    struct foreach_optimization_env {
        std::vector<std::unique_ptr<statement>*> m_beginForeach;
        std::vector<std::unique_ptr<statement>*> m_endForeach;
        std::vector<std::variant<std::unique_ptr<statement>*, expr_uptr*>> m_darrayAt;
        std::vector<std::unique_ptr<statement>*> m_for;

        void check_action(std::unique_ptr<expression>* expr);
        void check_action(std::unique_ptr<statement>* stmt);
    };

}