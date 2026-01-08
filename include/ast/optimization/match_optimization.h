#pragma once

#include "ast/statement.h"


namespace dconstruct::ast {

    struct match_optimization_env {
        //std::vector<std::unique_ptr<statement>*> m_resultDeclarations;
        std::unique_ptr<statement>* m_resultDeclaration;
        //std::unique_ptr<statement>* m_checkDeclaration;
        std::string m_checkIdentifier;
        std::unique_ptr<statement>* m_outerIf;
        std::unique_ptr<expression>* m_checkVar;
        std::unique_ptr<expression>* m_precondition;
        std::vector<std::unique_ptr<expression>*> m_patterns;
        std::vector<std::unique_ptr<expression>*> m_matches;
        bool m_checkingCondition;
        u16 m_currentAssignIdx;

        void check_action(std::unique_ptr<expression>* expr);
        void check_action(std::unique_ptr<statement>* stmt);
    };
}