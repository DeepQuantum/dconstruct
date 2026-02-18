#pragma once

#include "ast/ast_source.h"
#include "state_script_lambda.h"
#include <string>
#include <vector>

namespace dconstruct::ast {

    struct state_script_track : public ast_element {

        state_script_track(std::string name, std::vector<state_script_lambda> lambdas) noexcept
            : m_name(std::move(name)), m_lambdas(std::move(lambdas)) {}

        void pseudo_c(std::ostream& os) const final;
        void pseudo_py(std::ostream& os) const final;
        void pseudo_racket(std::ostream& os) const final;

        [[nodiscard]] bool equals(const state_script_track& rhs) const noexcept;

        std::string m_name;
        std::vector<state_script_lambda> m_lambdas;
    };

    [[nodiscard]] bool operator==(const state_script_track& lhs, const state_script_track& rhs) noexcept;

}
