#pragma once

#include "base.h"
#include "ast/ast.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/foreach_optimization.h"

namespace dconstruct::ast {
    struct variable_declaration : public statement {

        explicit variable_declaration(ast::full_type type, std::string id_name) noexcept :
        m_type(std::move(type)), m_identifier(std::move(id_name)), m_init(nullptr) {}; 

        explicit variable_declaration(ast::full_type type, std::string id_name, expr_uptr&& init) noexcept :
        m_type(std::move(type)), m_identifier(std::move(id_name)), m_init(std::move(init)) {}; 

        explicit variable_declaration(ast::full_type type, std::string id_name, const ast::primitive_value& init) noexcept :
        m_type(std::move(type)), m_identifier(std::move(id_name)), m_init(std::make_unique<ast::literal>(init)) {};

        [[nodiscard]] std::vector<semantic_check_error> check_semantics(type_environment& env) const noexcept final { return {}; }

        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;
        MATCH_OPTIMIZATION_ACTION match_optimization_pass(match_optimization_env& env) noexcept final;

        void pseudo_c(std::ostream& os) const final;

        void pseudo_py(std::ostream& os) const final;

		void pseudo_racket(std::ostream& os) const final;

        [[nodiscard]] bool equals(const statement& rhs) const noexcept final;

        [[nodiscard]] std::unique_ptr<statement> clone() const noexcept final;

        [[nodiscard]] inline const expression* get_init_ptr() const noexcept {
            return m_init.get();
        } 

        ast::full_type m_type;
        std::string m_identifier;
        expr_uptr m_init;
    };

}