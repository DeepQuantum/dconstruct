#include "decompilation/decomp_function.h"
#include <sstream>

namespace dconstruct::dcompiler {

decomp_function::decomp_function(const function_disassembly *func, const BinaryFile &current_file) : 
    m_disassembly{func}, m_file{current_file}, m_graph{func}
{
    m_blockStack.push(std::ref(m_baseBlock));
    m_transformableExpressions.resize(49);
    for (u32 i = 49; i < 128; ++i) {
        m_transformableExpressions.push_back(std::make_unique<ast::identifier>(compiler::token{ compiler::token_type::IDENTIFIER, "arg_" + std::to_string(i - 49)}));
    }

    for (u32 i = 0; i < func->m_stackFrame.m_registerArgs.size(); ++i) {
        const auto& arg_type = func->m_stackFrame.m_registerArgs.at(i);
        const std::string type_name = ast::type_to_declaration_string(arg_type);
        m_arguments.push_back(ast::variable_declaration(type_name, "arg_" + std::to_string(i)));
    }

    emit_node(m_graph[0], m_graph.get_return_node().m_startLine);
    parse_basic_block(m_graph.get_return_node());
}

decomp_function::decomp_function(const function_disassembly *func, const BinaryFile &current_file, const SymbolTable &table) : decomp_function(func, current_file) {
    m_symbolTable = table;
}; 

[[nodiscard]] std::string decomp_function::to_string() const {
    std::ostringstream out;
    out << ast::type_to_declaration_string(m_returnType) << ' ' << m_id << '(';
    for (u32 i = 0; i < m_arguments.size(); ++i) {
        out << m_arguments[i].m_typeName << ' ' << m_arguments[i].m_identifier;
        if (i != m_arguments.size() - 1) {
            out << ", ";
        }
    }
    out << ") ";
    out << m_baseBlock;
    return out.str();
}

void decomp_function::parse_basic_block(const control_flow_node &node) {
    static const compiler::token plus = compiler::token{ compiler::token_type::PLUS, "+" };
    static const compiler::token minus = compiler::token{ compiler::token_type::MINUS, "-" };
    static const compiler::token multiply = compiler::token{ compiler::token_type::STAR, "*" };
    static const compiler::token divide = compiler::token{ compiler::token_type::SLASH, "/" };

    std::set<u32> registers_to_emit = m_graph.get_variant_registers(node.m_startLine);

    for (const auto &line : node.m_lines) {
        const Instruction &istr = line.m_instruction;

        expr_uptr generated_expression = nullptr;

        switch(istr.opcode) {
            case Opcode::IAdd: 
            case Opcode::FAdd: generated_expression = apply_binary_op<ast::add_expr>(istr, plus); break;
            case Opcode::ISub:
            case Opcode::FSub: generated_expression = apply_binary_op<ast::sub_expr>(istr, minus); break;
            case Opcode::IMul:
            case Opcode::FMul: generated_expression = apply_binary_op<ast::mul_expr>(istr, multiply); break;
            case Opcode::IDiv:
            case Opcode::FDiv: generated_expression = apply_binary_op<ast::div_expr>(istr, divide); break;

            case Opcode::IAddImm: generated_expression = apply_binary_op_imm<ast::add_expr>(istr, plus); break;
            case Opcode::IMulImm: generated_expression = apply_binary_op_imm<ast::mul_expr>(istr, multiply); break;
            case Opcode::ISubImm: generated_expression = apply_binary_op_imm<ast::sub_expr>(istr, minus); break;
            case Opcode::IDivImm: generated_expression = apply_binary_op_imm<ast::div_expr>(istr, divide); break;

            case Opcode::IEqual:
            case Opcode::FEqual: generated_expression = apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::EQUAL_EQUAL, "=="}); break;
            case Opcode::INotEqual:
            case Opcode::FNotEqual: generated_expression = apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::BANG_EQUAL, "!="}); break;
            case Opcode::ILessThan:
            case Opcode::FLessThan: generated_expression = apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::LESS, "<"}); break;
            case Opcode::ILessThanEqual:
            case Opcode::FLessThanEqual: generated_expression = apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::LESS_EQUAL, "<="}); break;
            case Opcode::IGreaterThan:
            case Opcode::FGreaterThan: generated_expression = apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::GREATER, ">"}); break;
            case Opcode::IGreaterThanEqual:
            case Opcode::FGreaterThanEqual: generated_expression = apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::GREATER_EQUAL, ">="}); break;

            case Opcode::OpLogNot: generated_expression = apply_unary_op<ast::logical_not_expr>(istr, compiler::token{compiler::token_type::BANG, "!"}); break;
            case Opcode::OpBitNot: generated_expression = apply_unary_op<ast::bitwise_not_expr>(istr, compiler::token{compiler::token_type::TILDE, "~"}); break;
            case Opcode::INeg:
            case Opcode::FNeg: generated_expression = apply_unary_op<ast::negate_expr>(istr, compiler::token{compiler::token_type::MINUS, "-"}); break;

            case Opcode::LoadU16Imm: generated_expression = std::make_unique<ast::literal>(static_cast<u16>(istr.operand1 | static_cast<u16>(istr.operand2) << 8)); break;
            case Opcode::LoadStaticInt: generated_expression = std::make_unique<ast::literal>(m_symbolTable.value().get().first.get<i32>()); break;
            case Opcode::LoadStaticFloat: generated_expression = std::make_unique<ast::literal>(m_symbolTable.value().get().first.get<f32>()); break;
            case Opcode::LoadStaticI32Imm: generated_expression = std::make_unique<ast::literal>(m_symbolTable.value().get().first.get<i32>()); break;

            case Opcode::Call:
            case Opcode::CallFf: {
                expr_uptr call = make_call(istr);
                if (m_graph.register_gets_read_before_overwrite(node.m_startLine, istr.destination, line.m_location - node.m_startLine)) {
                    generated_expression = std::move(call);
                } else {
                    append_to_current_block(std::make_unique<ast::expression_stmt>(std::move(call)));
                    generated_expression = nullptr;
                }
                break;
            }
            

            case Opcode::LoadStaticPointerImm: {
                std::string string = m_symbolTable.value().get().first.get<const char*>(istr.operand1);
                generated_expression = std::make_unique<ast::literal>(std::move(string));
                break;
            }
            case Opcode::LoadStaticU64Imm:
            case Opcode::LookupPointer: {
                const sid64 sid = m_symbolTable.value().get().first.get<sid64>(istr.operand1);
                const std::string& name = m_file.m_sidCache.at(sid);
                sid_literal sid_literal = {sid, name};
                expr_uptr lit = std::make_unique<ast::literal>(std::move(sid_literal));
                if (std::holds_alternative<ast::function_type>(m_symbolTable.value().get().second[istr.operand1])) {
                    generated_expression = std::move(lit);
                }
                else {
                    m_transformableExpressions[istr.destination] = std::move(lit);
                }
                break;
            }

            case Opcode::CastInteger: {
                generated_expression = make_cast_to_int(istr);
                break;
            }
            case Opcode::CastFloat: {
                generated_expression = make_cast_to_float(istr);
                break;
            }

            case Opcode::Move: {
                generated_expression = m_transformableExpressions[istr.operand1]->clone();
                break;
            }

            case Opcode::LoadFloat:
            case Opcode::LoadI32: {
                generated_expression = make_dereference(istr);
                break;
            }

            case Opcode::AssertPointer:
            case Opcode::BreakFlag:
            case Opcode::Branch:
            case Opcode::BranchIf:
            case Opcode::BranchIfNot: {
                continue;
            }

            case Opcode::Return: insert_return(istr.destination); break;

            default: {
                throw std::runtime_error("");
            }
        }
        if (generated_expression != nullptr) {
            if (registers_to_emit.contains(istr.destination) || generated_expression->complexity() > 5) {
                load_expression_into_var(istr.destination, std::move(generated_expression));
            }
            else {
                m_transformableExpressions[istr.destination] = std::move(generated_expression);
            }
        }
    }
}

void decomp_function::emit_node(const control_flow_node& node, node_id stop_node) {
    const node_id current_node_id = node.m_startLine;
    if (m_parsedNodes.contains(current_node_id)) {
        return;
    }
    m_parsedNodes.insert(current_node_id);

    if (current_node_id == stop_node) {
        return;
    }

    parse_basic_block(node);
    const auto& last_line = node.get_last_line();

    if (const auto loop = m_graph.get_loop_with_head(last_line.m_location + 1)) {
        const control_flow_node& head_node = m_graph[last_line.m_location + 1];
        parse_basic_block(head_node);
        insert_loop_head(loop->get(), head_node.m_lines.back().m_instruction.operand1);
        emit_node(m_graph[head_node.get_last_line().m_location + 1], head_node.m_startLine);
        m_blockStack.pop();
    } else if (last_line.m_instruction.opcode == Opcode::BranchIf || last_line.m_instruction.opcode == Opcode::BranchIfNot) {
        const node_id idom = m_graph.get_immediate_postdominators().at(current_node_id);
        node_id proper_successor, proper_destination;
        expr_uptr condition = make_condition(node, proper_successor, proper_destination);
        auto then_block = std::make_unique<ast::block>();
        auto else_block = std::make_unique<ast::block>();

        m_blockStack.push(*then_block);
        emit_node(m_graph[proper_successor], idom);
        m_blockStack.pop();

        m_blockStack.push(*else_block);
        emit_node(m_graph[proper_destination], idom);
        m_blockStack.pop();

        stmnt_uptr _else = nullptr;
        if (else_block->m_statements.size() == 1 && dynamic_cast<ast::if_stmt*>(else_block->m_statements[0].get())) {
            _else = std::move(else_block->m_statements[0]);
        } else if (!else_block->m_statements.empty()) {
            _else = std::move(else_block);
        }

        stmnt_uptr full_if = std::make_unique<ast::if_stmt>(std::move(condition), std::move(then_block), std::move(_else));

        append_to_current_block(std::move(full_if));

        emit_node(m_graph[idom], stop_node);
    }
}

void decomp_function::load_expression_into_var(const u32 dst, expr_uptr&& expr) {
    auto id = std::make_unique<ast::identifier>(compiler::token{ compiler::token_type::IDENTIFIER, "var_" + std::to_string(dst)});
    const std::string name = id->m_name.m_lexeme;
    const ast::full_type type = expr->get_type(m_env).value_or(std::monostate());
    const std::string type_name = type_to_declaration_string(type);

    auto var_declaration = std::make_unique<ast::variable_declaration>(type_name, name, std::move(expr));

    append_to_current_block(std::move(var_declaration));
    m_transformableExpressions[dst] = std::move(id);
}

[[nodiscard]] expr_uptr decomp_function::make_call(const Instruction& istr) {
    expr_uptr callee = m_transformableExpressions[istr.destination]->clone();  
    std::vector<expr_uptr> args;

    for (u8 i = 0; i < istr.operand2; ++i) {
        args.push_back(m_transformableExpressions[49 + i]->simplify());
    }

    return std::make_unique<ast::call_expr>(compiler::token{ compiler::token_type::_EOF, "" }, std::move(callee), std::move(args));;
}


[[nodiscard]] expr_uptr decomp_function::make_cast_to_int(const Instruction& istr) {
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


[[nodiscard]] expr_uptr decomp_function::make_cast_to_float(const Instruction& istr) {
    const ast::literal* old_lit = dynamic_cast<ast::literal*>(m_transformableExpressions[istr.operand1].get());
    if (old_lit == nullptr) {
        const auto& op2 = m_transformableExpressions[istr.operand1];
        return std::make_unique<ast::cast_expr>(
            compiler::token{ compiler::token_type::IDENTIFIER, "32"}, 
            std::make_unique<ast::identifier>("f32"), 
            is_binary(op2.get()) ? std::make_unique<ast::grouping>(op2->clone()) : op2->clone()
        );
    }
    else {
        return std::make_unique<ast::literal>(static_cast<f32>(std::get<i64>(old_lit->m_value)));
    }
}

[[nodiscard]] expr_uptr decomp_function::make_condition(const control_flow_node& node, node_id& proper_successor, node_id& proper_destination) {
    const auto& last_line = node.get_last_line();
    const auto& successor_last_line = m_graph[node.get_direct_successor()].get_last_line();
    expr_uptr current_condition = m_transformableExpressions[last_line.m_instruction.operand1]->clone();
    expr_uptr condition = nullptr;
    proper_successor = node.get_direct_successor();
    proper_destination = last_line.m_instruction.destination;
    if (last_line.m_instruction.destination == successor_last_line.m_location) {
        m_parsedNodes.insert(proper_successor);
        parse_basic_block(m_graph[proper_successor]);
        condition = std::make_unique<ast::logical_expr>(
            compiler::token{ compiler::token_type::AMPERSAND_AMPERSAND, "&&" },
            std::move(current_condition),
            m_transformableExpressions[successor_last_line.m_instruction.operand1]->clone()
        );
        proper_successor = m_graph[proper_successor].get_direct_successor();
        proper_destination = successor_last_line.m_instruction.destination;
    } else {
        condition = current_condition->clone();
    }
    return condition;
}

[[nodiscard]] expr_uptr decomp_function::make_dereference(const Instruction& istr) {
    expr_uptr load = std::make_unique<ast::dereference_expr>(compiler::token{ compiler::token_type::STAR, "*" }, std::move(m_transformableExpressions[istr.operand1]));
    return load;
}

void decomp_function::insert_return(const u32 dest) {
    append_to_current_block(std::make_unique<ast::return_stmt>(std::move(m_transformableExpressions[dest])));
}


void decomp_function::insert_loop_head(const control_flow_loop& loop, const u32 conditional_check_location) {
    std::unique_ptr<ast::while_stmt> _while = std::make_unique<ast::while_stmt>(m_transformableExpressions[conditional_check_location]->clone(), std::make_unique<ast::block>());

    append_to_current_block(std::move(_while));
    
    auto& while_ref = static_cast<ast::while_stmt&>(*m_blockStack.top().get().m_statements.back().get());

    m_blockStack.push(static_cast<ast::block&>(*while_ref.m_body.get()));
}

}