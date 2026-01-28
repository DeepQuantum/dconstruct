#pragma once

#include "ast/statement.h"
#include "ast/optimization/var_optimization.h"
#include "ast/optimization/match_optimization.h"
#include "ast/optimization/foreach_optimization.h"
#include <list>


namespace dconstruct::ast {
    struct block : public statement {
        explicit block() noexcept : m_statements{} {};
        explicit block(std::list<stmnt_uptr>&& stmnts) noexcept : m_statements{ std::move(stmnts) } {};
        explicit block(std::vector<stmnt_uptr>&& stmnts) noexcept {
            for (auto& stmnt : stmnts) {
                m_statements.push_back(std::move(stmnt));
            }
        }

        block(const block& other) noexcept = delete;
        block& operator=(const block& other) noexcept = delete;
        block(block&& other) noexcept = default;
        block& operator=(block&& other) noexcept = default;

        void pseudo_c(std::ostream&) const final;
        void pseudo_py(std::ostream&) const final;
		void pseudo_racket(std::ostream&) const final;
        [[nodiscard]] bool equals(const statement& rhs) const noexcept final;
        [[nodiscard]] std::unique_ptr<statement> clone() const noexcept final;
        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;
        MATCH_OPTIMIZATION_ACTION match_optimization_pass(match_optimization_env& env) noexcept final;
        [[nodiscard]] const statement* inlineable_else_statement() const noexcept final;
        [[nodiscard]] bool is_dead_code() const noexcept final;
        [[nodiscard]] std::vector<semantic_check_error> check_semantics(compilation::scope& env) const noexcept final;
        [[nodiscard]] emission_err emit_dc(compilation::function& fn, compilation::global_state& global) const noexcept final;

        void clear_dead_statements() noexcept;


        std::list<stmnt_uptr> m_statements;
       // std::vector<u32> m_removedStatementsIndices;
    };
}