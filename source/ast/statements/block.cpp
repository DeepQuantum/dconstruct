#include "ast/statements/block.h"
#include "ast/statements/variable_declaration.h"

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

void block::decomp_optimization_pass(second_pass_env& env) noexcept {
    second_pass_env new_env{std::move(env)};

    for (auto& statement : m_statements) {
        statement->decomp_optimization_pass(env);
    }

    for (auto& [name, expression] : new_env->m_values) {

        assert(dynamic_cast<ast::variable_declaration*>(expression.m_declareSite->get()));

        if (expression.m_uses == 0) {
            ast::variable_declaration* decl = static_cast<ast::variable_declaration*>(expression.m_declareSite->get());

            assert(dynamic_cast<ast::call_expr*>(decl->m_init.get()));

            *expression.m_declareSite = std::make_unique<ast::expression_stmt>(std::move(decl->m_init)); 
        } else if (expression.m_uses == 1) {
            ast::variable_declaration* decl = static_cast<ast::variable_declaration*>(expression.m_declareSite->get());
            
            delete expression.m_firstUsageSite;
            expression.m_firstUsageSite = decl->m_init.release();

            expression.m_declareSite->reset();
            expression.m_declareSite = nullptr;
        }
    }
}

}