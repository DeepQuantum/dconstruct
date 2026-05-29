#pragma once

#include "ast/ast_source.h"
#include "ast/function_definition.h"
#include "disassembly/instructions.h"
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace dconstruct::ast {

    using state_script_lambda = std::variant<function_definition, std::shared_ptr<dconstruct::function_disassembly>>;

    struct state_script_track : public ast_element {

        state_script_track(std::string name, std::vector<function_definition> lambdas) noexcept
            : m_name(std::move(name)) {
            m_lambdas.reserve(lambdas.size());
            for (auto& lambda : lambdas) {
                m_lambdas.emplace_back(std::move(lambda));
            }
        }

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
