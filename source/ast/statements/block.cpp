#include "ast/statements/block.h"
#include "ast/statements/variable_declaration.h"

#include <iostream>

namespace dconstruct::ast {

void block::pseudo_c(std::ostream& os) const {
    if (m_statements.empty()) {
        os << "{}" << '\n';
    } else {
        os << "{\n";
        os << indent_more;
        for (const auto& stmnt : m_statements) {
            os << indent << *stmnt << '\n';
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
    if (rhs_ptr == nullptr) {
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

VAR_FOLDING_ACTION block::decomp_optimization_pass(second_pass_env& env) noexcept {
    second_pass_env new_env{&env};

   // std::vector<stmnt_uptr> copies;
    static int count {};
    for (auto& statement : m_statements) {
        std::cout << count++ << "\n";
        if (statement) {
            statement::check_optimization(&statement, new_env);
        }
    }
    for (auto& [name, expression] : new_env.m_values) {
        if (expression.m_reads.size() == 0) {
            auto* decl = static_cast<ast::variable_declaration*>(expression.m_declaration->get());

            if (!decl->m_init) {
                *expression.m_declaration = nullptr;
            } else{ 
                *expression.m_declaration = std::make_unique<ast::expression_stmt>(std::move(decl->m_init)); 
            }

            for (auto* write : expression.m_writes) {
                assert(dynamic_cast<ast::assign_expr*>(write->get()));
                auto* assignment = static_cast<ast::assign_expr*>(write->get()); 
                *write = std::move(assignment->m_rhs);
            }
        } else if (expression.m_reads.size() == 1 && expression.m_writes.size() < 2) {
            auto* decl = static_cast<ast::variable_declaration*>(expression.m_declaration->get());

            auto& init = decl->m_init;

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

    return VAR_FOLDING_ACTION::NONE;
}

}