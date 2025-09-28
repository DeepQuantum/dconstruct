#include "decompilation/expression_frame.h"

namespace dconstruct::dcompiler {

void expression_frame::load_expression_into_var(const u32 dst, expr_uptr&& expr) {
    auto id = std::make_unique<ast::identifier>(compiler::token{ compiler::token_type::IDENTIFIER, "var_" + std::to_string(dst)});
    const std::string name = id->m_name.m_lexeme;
    const ast::full_type type = expr->get_type(m_env).value_or(ast::primitive_type{ast::primitive_kind::UNKNOWN});
    const std::string type_name = type_to_declaration_string(type);

    auto var_declaration = std::make_unique<ast::variable_declaration>(type_name, name, std::move(expr));

    append_to_current_block(std::move(var_declaration));
    m_transformableExpressions[dst] = std::move(id);
}

[[nodiscard]] expr_uptr expression_frame::call(const Instruction& istr) {
    expr_uptr callee = m_transformableExpressions[istr.destination]->clone();  
    std::vector<expr_uptr> args;

    for (u8 i = 0; i < istr.operand2; ++i) {
        args.push_back(m_transformableExpressions[49 + i]->simplify());
    }

    return std::make_unique<ast::call_expr>(compiler::token{ compiler::token_type::_EOF, "" }, std::move(callee), std::move(args));;
}

[[nodiscard]] expr_uptr expression_frame::cast_to_int(const Instruction& istr) {
    const ast::literal* old_lit = dynamic_cast<ast::literal*>(m_transformableExpressions[istr.operand1].get());
    if (old_lit == nullptr) {
        const auto& op2 = m_transformableExpressions[istr.operand1];
        return std::make_unique<ast::cast_expr>(
            compiler::token{ compiler::token_type::IDENTIFIER, "i64"}, 
            std::make_unique<ast::identifier>("i64"), 
            is_binary(op2.get()) ? std::make_unique<ast::grouping>(op2->clone()) : op2->clone()
        );
    }
    else {
        return std::make_unique<ast::literal>(static_cast<i32>(std::get<f32>(old_lit->m_value)));
    }
}

[[nodiscard]] expr_uptr expression_frame::cast_to_float(const Instruction& istr) {
    const ast::literal* old_lit = dynamic_cast<ast::literal*>(m_transformableExpressions[istr.operand1].get());
    if (old_lit == nullptr) {
        const auto& op2 = m_transformableExpressions[istr.operand1];
        return std::make_unique<ast::cast_expr>(
            compiler::token{ compiler::token_type::IDENTIFIER, "f32"}, 
            std::make_unique<ast::identifier>("f32"), 
            is_binary(op2.get()) ? std::make_unique<ast::grouping>(op2->clone()) : op2->clone()
        );
    }
    else {
        return std::make_unique<ast::literal>(static_cast<i32>(std::get<f32>(old_lit->m_value)));
    }
}

[[nodiscard]] expr_uptr expression_frame::emit_condition(const control_flow_node& node) {
    const auto& last_line = node.get_last_line();
    // const auto& successor_last_line = graph[node.get_direct_successor()].get_last_line();
    // if ()
    expr_uptr& current_condition = m_transformableExpressions[last_line.m_instruction.operand1];
    return current_condition->clone();
}

[[nodiscard]] expr_uptr expression_frame::load_with_dereference(const Instruction& istr) {
    expr_uptr load = std::make_unique<ast::dereference_expr>(compiler::token{ compiler::token_type::STAR, "*" }, std::move(m_transformableExpressions[istr.operand1]));
    return load;
}

ast::return_stmt& expression_frame::insert_return(const u32 dest) {
    append_to_current_block(std::make_unique<ast::return_stmt>(std::move(m_transformableExpressions[dest])));
    return static_cast<ast::return_stmt&>(*m_blockStack.top().get().m_statements.back().get());
}

ast::while_stmt& expression_frame::insert_loop_head(const control_flow_loop& loop, const u32 conditional_check_location) {
    std::unique_ptr<ast::while_stmt> _while = std::make_unique<ast::while_stmt>(m_transformableExpressions[conditional_check_location]->clone(), std::make_unique<ast::block>());

    append_to_current_block(std::move(_while));
    
    auto& while_ref = static_cast<ast::while_stmt&>(*m_blockStack.top().get().m_statements.back().get());

    m_blockStack.push(static_cast<ast::block&>(*while_ref.m_body.get()));
    
    return while_ref;
}



}