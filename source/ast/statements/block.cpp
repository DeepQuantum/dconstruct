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

bool block::decomp_optimization_pass(second_pass_env& env) noexcept {
    second_pass_env new_env{&env};

   // std::vector<stmnt_uptr> copies;

    for (auto& statement : m_statements) {
        if (statement) {
            statement::check_optimization(&statement, new_env);
        }
    }
    for (auto& [name, expression] : new_env.m_values) {
        if (expression.m_uses == 0) {
            ast::variable_declaration* decl = static_cast<ast::variable_declaration*>(expression.m_original->get());

            assert(dynamic_cast<ast::call_expr*>(decl->m_init.get()));

            *expression.m_original = std::make_unique<ast::expression_stmt>(std::move(decl->m_init)); 
        } else if (expression.m_uses == 1) {
            ast::variable_declaration* decl = static_cast<ast::variable_declaration*>(expression.m_original->get());

            std::unique_ptr<ast::expression>& init = decl->m_init;

            *expression.m_replace = std::move(init);
            *expression.m_original = nullptr;
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

    return false;
}

}