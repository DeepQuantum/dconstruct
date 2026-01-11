#include "ast/statements/block.h"
#include "ast/statements/variable_declaration.h"
#include "ast/statements/foreach.h"

#include <iostream>

namespace dconstruct::ast {

void block::pseudo_c(std::ostream& os) const {
    if (m_statements.empty()) {
        os << "{}" << '\n';
    } else {
        os << "{\n";
        os << indent_more;
        // if (!m_removedStatementsIndices.empty()) {
        //     u32 index_counter = 0;
        //     u32 removed_index = m_removedStatementsIndices[index_counter];
        //     for (u32 i = 0; i < m_statements.size(); ++i) {
        //         if (i == removed_index) {
        //             if (index_counter < m_removedStatementsIndices.size() - 1) {
        //                 removed_index = m_removedStatementsIndices[index_counter++];
        //             } else {
        //                 removed_index = -1;
        //             }
        //             continue;
        //         }
        //         os << indent << *m_statements[i] << '\n';
        //     }
        // } else {
            for (const auto& stmnt : m_statements) {
                os << indent << *stmnt << '\n';
            }
       // }
        os << indent_less;
        os << indent << '}';
    }
}

void block::clear_dead_statements() noexcept {
    m_statements.remove_if([](const auto& stmt) {
        return !stmt || stmt->is_dead_code();
    });
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

[[nodiscard]] const statement* block::inlineable_else_statement() const noexcept {
    if (m_statements.size() == 1) {
        return m_statements.front()->inlineable_else_statement();
    }
    return nullptr;
}

[[nodiscard]] bool block::is_dead_code() const noexcept {
    return std::all_of(m_statements.begin(), m_statements.end(), [](const auto& stmnt) { return stmnt->is_dead_code(); });
}

VAR_OPTIMIZATION_ACTION block::var_optimization_pass(var_optimization_env& env) noexcept {
    var_optimization_env new_env{&env.m_env};
    for (auto& statement : m_statements) {
        if (statement) {
            new_env.check_action(&statement);
        }
    }
    for (auto& [name, expression] : new_env.m_env.m_values) {
        if (expression.m_reads.size() == 0) {
            auto& decl = static_cast<ast::variable_declaration&>(**expression.m_declaration);

            if (!decl.m_init) {
                *expression.m_declaration = nullptr;
            } else{ 
                *expression.m_declaration = std::make_unique<ast::expression_stmt>(std::move(decl.m_init)); 
            }

            for (auto* assign : expression.m_assigns) {
                assert(dynamic_cast<ast::assign_expr*>(assign->get()));
                auto& assignment = static_cast<ast::assign_expr&>(**assign); 
                *assign = std::move(assignment.m_rhs);
            }
        } else if (expression.m_reads.size() == 1 && expression.m_assigns.size() < 2) {
            auto& decl = static_cast<ast::variable_declaration&>(**expression.m_declaration);

            auto& init = decl.m_init;

            if (init) {
                *expression.m_reads[0] = std::move(init);
                *expression.m_declaration = nullptr;
            }
        }
    }

    clear_dead_statements();

    return VAR_OPTIMIZATION_ACTION::NONE;
}

FOREACH_OPTIMIZATION_ACTION block::foreach_optimization_pass(foreach_optimization_env& env) noexcept {
    for (auto& statement : m_statements) {
        if (statement) {
            const auto prev_size = env.m_endForeach.size();
            env.check_action(&statement);
            if (env.m_endForeach.size() > prev_size) {
                auto& old_for = static_cast<for_stmt&>(**env.m_for.back());
                auto& old_body_ptr = old_for.m_body;
                auto& old_body = static_cast<block&>(*old_for.m_body);

                auto& decl = static_cast<ast::binary_expr&>(*old_for.m_condition);
                // auto& count = static_cast<call_expr&>(*decl.m_rhs);
                // auto& iterable = count.m_arguments[0];

                expr_uptr iterable;
                if (auto* arg_ptr = decl.m_rhs->get_first_argument()) {
                    iterable = std::move(*arg_ptr);
                } else {
                    for (auto& statement : m_statements) {
                        if (statement->foreach_optimization_pass(env) == FOREACH_OPTIMIZATION_ACTION::ITERABLE_COUNT) {
                            assert(dynamic_cast<variable_declaration*>(statement.get()));
                            auto& count_decl = static_cast<variable_declaration&>(*statement);
                            auto& init = count_decl.m_init;
                            assert(dynamic_cast<call_expr*>(init.get()));
                            auto* arg_ptr = init->get_first_argument();
                            assert(arg_ptr);
                            iterable = arg_ptr->get()->clone();
                            break;
                        }
                    }
                }
                assert(iterable && "should have an iterable here...");

                std::string iter_var;
                iter_var.reserve(sizeof("element"));

                if (std::holds_alternative<expr_uptr*>(env.m_iterableAt.back())) {
                    // e.g. foo(var_x, darray-at(arr, i));
                    iter_var = "element";
                    *std::get<expr_uptr*>(env.m_iterableAt.back()) = std::make_unique<identifier>(iter_var);
                } else {
                    // e.g. u64 element = darray-at(arr, i);
                    assert(std::holds_alternative<stmnt_uptr*>(env.m_iterableAt.back()));
                    assert(dynamic_cast<variable_declaration*>(std::get<stmnt_uptr*>(env.m_iterableAt.back())->get()));
                    auto& loop_decl = static_cast<variable_declaration&>(**std::get<stmnt_uptr*>(env.m_iterableAt.back()));
                    iter_var = loop_decl.m_identifier;
                    *std::get<stmnt_uptr*>(env.m_iterableAt.back()) = nullptr;
                }


                auto for_each = std::make_unique<foreach_stmt>(parameter{std::monostate(), std::move(iter_var)}, std::move(iterable), std::move(old_body_ptr));

                *env.m_beginForeach.back() = nullptr;
                env.m_beginForeach.pop_back();

                *env.m_endForeach.back() = nullptr;
                env.m_endForeach.pop_back();


                env.m_iterableAt.pop_back();

                *env.m_for.back() = std::move(for_each);
                env.m_for.pop_back();
            }
        }
    }

    clear_dead_statements();

    return FOREACH_OPTIMIZATION_ACTION::NONE;
}

MATCH_OPTIMIZATION_ACTION block::match_optimization_pass(match_optimization_env& env) noexcept {
    if (m_statements.size() == 1) {
        if (m_statements.front()->match_optimization_pass(env) == MATCH_OPTIMIZATION_ACTION::RESULT_VAR_ASSIGNMENT) {
            return MATCH_OPTIMIZATION_ACTION::RESULT_VAR_ASSIGNMENT;
        }
    } else {
        env = match_optimization_env{};
        for (auto& statement : m_statements) {
            if (!statement) {
                continue;
            }
            const auto action = statement->match_optimization_pass(env);
            if (action == MATCH_OPTIMIZATION_ACTION::RESULT_VAR_ASSIGNMENT && env.m_matches.size() > 2 && env.m_matches.size() - 1 == env.m_patterns.size()) {
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

    //env.m_resultDeclarations.clear();
    env.m_resultDeclaration = nullptr;

    clear_dead_statements();

    return MATCH_OPTIMIZATION_ACTION::NONE;
}

}