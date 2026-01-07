#pragma once
#include "ast/statement.h"


namespace dconstruct::ast {

    struct foreach_optimization_env {
        std::vector<stmnt_uptr*> m_beginForeach;
        std::vector<stmnt_uptr*> m_endForeach;
        std::vector<std::variant<stmnt_uptr*, expr_uptr*>> m_darrayAt;
        std::vector<stmnt_uptr*> m_for;

        void check_action(std::unique_ptr<expression>* expr);
        void check_action(std::unique_ptr<statement>* stmt);
    };

}