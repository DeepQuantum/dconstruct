#include "ast/statements/block.h"
#include "ast/statements/variable_declaration.h"

#include <iostream>

namespace dconstruct::ast {

void block::pseudo_c(std::ostream& os) const {
    static int a{};
    if (m_statements.empty()) {
        os << "{}" << '\n';
    } else {
        os << "{\n";
        os << indent_more;
        if (!m_removedStatementsIndices.empty()) {
            u32 index_counter = 0;
            u32 removed_index = m_removedStatementsIndices[index_counter];
            for (u32 i = 0; i < m_statements.size(); ++i) {
                if (i == removed_index) {
                    if (index_counter < m_removedStatementsIndices.size() - 1) {
                        removed_index = m_removedStatementsIndices[index_counter++];
                    } else {
                        removed_index = -1;
                    }
                    continue;
                }
                os << indent << *m_statements[i] << '\n';
            }
        } else {
            for (const auto& stmnt : m_statements) {
                os << indent << *stmnt << '\n';
            }
        }
        os << indent_less;
        os << indent << '}';
    }
}


void block::pseudo_py(std::ostream& os) const {
    os << ':' << indent_more;
    for (const auto& stmnt : m_statements) {
        os << indent << *stmnt << '\n';
    } 
}

void block::pseudo_racket(std::ostream& os) const {
    os << "(begin\n";
    os << indent_more;
    for (const auto& stmnt : m_statements) {
        os << indent << *stmnt << '\n';
    } 
    os << indent_less;
    os << indent << ')';
}

[[nodiscard]] bool block::equals(const statement& rhs) const noexcept {
    const block* rhs_ptr = dynamic_cast<const block*>(&rhs);
    if (!rhs_ptr) {
        return false;
    }
    return m_statements == rhs_ptr->m_statements;
}

[[nodiscard]] std::unique_ptr<statement> block::clone() const noexcept {
    std::vector<stmnt_uptr> new_statements;
    new_statements.reserve(m_statements.size());
    for (const auto& statement : m_statements) {
        new_statements.emplace_back(statement->clone());
    }
    return std::make_unique<block>(std::move(new_statements));
}

VAR_OPTIMIZATION_ACTION block::var_optimization_pass(var_optimization_env& env) noexcept {
    var_optimization_env new_env{&env};

    for (auto& statement : m_statements) {
        if (statement) {
            statement::check_var_optimization(&statement, new_env);
        }
    }
    for (auto& [name, expression] : new_env.m_values) {
        if (expression.m_reads.size() == 0) {
            auto& decl = static_cast<ast::variable_declaration&>(**expression.m_declaration);

            if (!decl.m_init) {
                *expression.m_declaration = nullptr;
            } else{ 
                *expression.m_declaration = std::make_unique<ast::expression_stmt>(std::move(decl.m_init)); 
            }

            for (auto* write : expression.m_writes) {
                assert(dynamic_cast<ast::assign_expr*>(write->get()));
                auto& assignment = static_cast<ast::assign_expr&>(**write); 
                *write = std::move(assignment.m_rhs);
            }
        } else if (expression.m_reads.size() == 1 && expression.m_writes.size() < 2) {
            auto& decl = static_cast<ast::variable_declaration&>(**expression.m_declaration);

            auto& init = decl.m_init;

            if (init) {
                *expression.m_reads[0] = std::move(init);
                *expression.m_declaration = nullptr;
            }
        }
    }

    std::vector<stmnt_uptr> new_statements;
    new_statements.reserve(m_statements.size());

    for (auto& statement : m_statements) {
        if (statement) {
            new_statements.emplace_back(std::move(statement));
        }
    }

    m_statements = std::move(new_statements);

    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION block::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    for (auto& statement : m_statements) {
        if (statement) {
            statement::check_foreach_optimization(&statement, env);
        }
    }
    std::vector<stmnt_uptr> new_statements;
    new_statements.reserve(m_statements.size());

    for (auto& statement : m_statements) {
        if (statement) {
            new_statements.emplace_back(std::move(statement));
        }
    }

    m_statements = std::move(new_statements);
    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

MATCH_OPTIMIZATION_ACTION block::match_optimization_pass(match_optimization_env& env) noexcept {
    if (m_statements.size() == 1) {
        if (m_statements[0]->match_optimization_pass(env) == MATCH_OPTIMIZATION_ACTION::RESULT_VAR_ASSIGNMENT) {
            return MATCH_OPTIMIZATION_ACTION::RESULT_VAR_ASSIGNMENT;
        }
    } else {
        for (auto& statement : m_statements) {
            const auto action = statement->match_optimization_pass(env);
            if (action == MATCH_OPTIMIZATION_ACTION::RESULT_VAR_ASSIGNMENT && m_statements.size() == 1) {
                assert(env.m_matches.size() - 1 == env.m_patterns.size());
                std::vector<std::pair<expr_uptr, expr_uptr>> pairs;
                pairs.reserve(env.m_patterns.size());

                for (u32 i = 0; i < env.m_patterns.size(); ++i) {
                    pairs.emplace_back(std::move(*env.m_patterns[i]), std::move(*env.m_matches[i]));
                }

                auto match = std::make_unique<match_expr>(std::move(*env.m_checkVar), std::move(pairs), std::move(*env.m_matches.back()));

                statement = nullptr;
                assert(dynamic_cast<variable_declaration*>(env.m_resultDeclaration->get()));
                static_cast<variable_declaration&>(**env.m_resultDeclaration).m_init = std::move(match);
            } else if (action == MATCH_OPTIMIZATION_ACTION::RESULT_VAR_DECLARATION) {
                env.m_resultDeclaration = &statement;
            }
        }
    }

    std::vector<stmnt_uptr> new_statements;
    new_statements.reserve(m_statements.size());

    for (auto& statement : m_statements) {
        if (statement) {
            new_statements.emplace_back(std::move(statement));
        }
    }

    m_statements = std::move(new_statements);

    return MATCH_OPTIMIZATION_ACTION::NONE;
}

}