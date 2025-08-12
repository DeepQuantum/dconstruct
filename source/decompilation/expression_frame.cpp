#include "decompilation/expression_frame.h"

namespace dconstruct::dcompiler {

void expression_frame::move(const u32 dst, const u32 src) {

   /* auto id = std::make_unique<ast::identifier>(get_next_var_idx());
    auto rhs = m_transformableExpressions[src]->simplify();

    auto assign = std::make_unique<ast::assign_expr>(std::move(id), std::move(rhs));
    m_statements.emplace_back(std::make_unique<ast::expression_stmt>(assign->simplify()));

    m_transformableExpressions[dst] = std::move(assign);*/
}

void expression_frame::load_literal(const u8 dst, const ast::primitive_value& value) {
    auto literal = std::make_unique<ast::literal>(value);
    auto id = std::make_unique<ast::identifier>(get_next_var());

    const std::string name = id->get_name();
    const ast::primitive_kind type = literal->get_type();

    auto var_declaration = std::make_unique<ast::variable_declaration>(ast::kind_to_string(type), name, std::move(literal));

    m_statements.push_back(std::move(var_declaration));
    m_transformableExpressions[dst] = std::move(id);
}

}