#include "decompilation/decomp_function.h"
#include <sstream>
#include <iostream>

namespace dconstruct::dcompiler{

decomp_function::decomp_function(const function_disassembly *func, const BinaryFile &current_file) :
    m_disassembly{func}, m_file{current_file}, m_graph{func}, m_symbolTable{func->m_stackFrame.m_symbolTable}
{
    std::cout << "parsing graph " << m_disassembly->m_id << '\n';
    m_graph.write_image(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\images\)" + m_disassembly->m_id + ".svg");
    m_blockStack.push(std::ref(m_baseBlock));
    m_transformableExpressions.resize(49);
    for (reg_idx i = 0; i < ARGUMENT_REGISTERS_IDX; ++i) {
        m_registersToVars.emplace(i, std::stack<std::unique_ptr<ast::identifier>>());
    }
    for (reg_idx i = ARGUMENT_REGISTERS_IDX; i < 128; ++i) {
        m_transformableExpressions.push_back(std::make_unique<ast::identifier>(compiler::token{ compiler::token_type::IDENTIFIER, "arg_" + std::to_string(i - ARGUMENT_REGISTERS_IDX)}));
        m_registersToVars.emplace(i, std::stack<std::unique_ptr<ast::identifier>>());
    }

    for (reg_idx i = 0; i < func->m_stackFrame.m_registerArgs.size(); ++i) {
        const auto& arg_type = func->m_stackFrame.m_registerArgs.at(i);
        const std::string type_name = ast::type_to_declaration_string(arg_type);
        m_arguments.push_back(ast::variable_declaration(type_name, "arg_" + std::to_string(i)));
    }

    emit_node(m_graph[0], m_graph.get_return_node().m_startLine);
    parse_basic_block(m_graph.get_return_node());
}

[[nodiscard]] std::string decomp_function::to_string() const {
    std::ostringstream out;
    out << ast::type_to_declaration_string(m_returnType) << ' ' << m_disassembly->m_id << '(';
    for (u8 i = 0; i < m_arguments.size(); ++i) {
        out << m_arguments[i].m_typeName << ' ' << m_arguments[i].m_identifier;
        if (i != m_arguments.size() - 1) {
            out << ", ";
        }
    }
    out << ") " << m_baseBlock;
    return out.str();
}

static const compiler::token plus = compiler::token{ compiler::token_type::PLUS, "+" };
static const compiler::token minus = compiler::token{ compiler::token_type::MINUS, "-" };
static const compiler::token multiply = compiler::token{ compiler::token_type::STAR, "*" };
static const compiler::token divide = compiler::token{ compiler::token_type::SLASH, "/" };

void decomp_function::parse_basic_block(const control_flow_node &node) {
    std::cout << "parsing node " << node.m_startLine << '\n';
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
            case Opcode::LoadStaticInt: generated_expression = std::make_unique<ast::literal>(m_symbolTable.first.get<i32>()); break;
            case Opcode::LoadStaticFloat: generated_expression = std::make_unique<ast::literal>(m_symbolTable.first.get<f32>()); break;
            case Opcode::LoadStaticI32Imm: generated_expression = std::make_unique<ast::literal>(m_symbolTable.first.get<i32>(istr.operand1 * 8)); break;

            case Opcode::Call:
            case Opcode::CallFf: {
                expr_uptr call = make_call(istr);
                if (m_graph.register_gets_read_before_overwrite(node.m_startLine, istr.destination, line.m_location - node.m_startLine)) {
                    //generated_expression = std::move(call);
                    load_expression_into_new_var(istr.destination, std::move(call));
                } else {
                    append_to_current_block(std::make_unique<ast::expression_stmt>(std::move(call)));
                    generated_expression = nullptr;
                }
                break;
            }
            
            case Opcode::LoadStaticFloatImm: {
                generated_expression = std::make_unique<ast::literal>(m_symbolTable.first.get<f32>(istr.operand1 * 8));
                break;
            }
            case Opcode::LoadStaticPointerImm: {
                generated_expression = std::make_unique<ast::literal>(m_symbolTable.first.get<const char*>(istr.operand1 * 8));
                break;
            }
            case Opcode::LoadStaticU64Imm:
            case Opcode::LookupPointer: {
                const sid64 sid = m_symbolTable.first.get<sid64>(istr.operand1 * 8);
                const std::string& name = m_file.m_sidCache.at(sid);
                expr_uptr lit = std::make_unique<ast::literal>(sid_literal{ sid, name });
                if (!std::holds_alternative<ast::function_type>(m_symbolTable.second[istr.operand1])) {
                    generated_expression = std::move(lit);
                }
                else {
                    m_transformableExpressions[istr.destination] = std::move(lit);
                    m_transformableExpressions[istr.destination]->set_type(m_symbolTable.second[istr.operand1]);
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
            case Opcode::LoadI32: 
            case Opcode::LoadU64:
            case Opcode::LoadU8:
            case Opcode::LoadU16: {
                generated_expression = apply_unary_op<ast::dereference_expr>(istr, compiler::token{compiler::token_type::STAR, "*"}); break;
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
            m_transformableExpressions[istr.destination] = std::move(generated_expression);
        }
    }
}

void decomp_function::emit_node(const control_flow_node& node, const node_id stop_node) {
    std::cout << "emitting node " << std::hex << node.m_startLine << '\n';

    const node_id current_node_id = node.m_startLine;
    if (current_node_id == stop_node) {
        return;
    }
    if (m_parsedNodes.contains(current_node_id)) {
        return;
    }
    m_parsedNodes.insert(current_node_id);

    


    parse_basic_block(node);
    const auto& last_line = node.get_last_line();

    if (const auto loop = m_graph.get_loop_with_head(last_line.m_location + 1)) {
        emit_loop(last_line, loop->get(), stop_node);
    } else if (last_line.m_instruction.opcode == Opcode::BranchIf || last_line.m_instruction.opcode == Opcode::BranchIfNot) {
        emit_branches(node, stop_node);
    }
}

void decomp_function::emit_loop(const function_disassembly_line &detect_node_last_line, const control_flow_loop &loop, const node_id stop_node) {
    const control_flow_node& head_node = m_graph[detect_node_last_line.m_location + 1];
    const node_id loop_entry = head_node.get_direct_successor();
    const node_id loop_tail = head_node.get_last_line().m_instruction.destination;
    auto loop_block = std::make_unique<ast::block>();
    std::set<reg_idx> regs_to_emit = m_graph.get_loop_variant_registers(head_node.m_startLine);
    const node_id idom = m_graph.get_immediate_postdominators().at(loop_tail);
    std::unordered_map<reg_idx, ast::full_type> regs_to_type;

    reg_idx loop_var_reg = head_node.m_lines[0].m_instruction.operand1;
    reg_idx loop_alternative_reg = head_node.get_last_line().m_instruction.operand1;
    regs_to_emit.erase(loop_var_reg);
    regs_to_emit.erase(loop_alternative_reg);
    const std::string name = "i";
    auto id = std::make_unique<ast::identifier>(name);

    auto declaration = std::make_unique<ast::variable_declaration>("u32", name, std::move(m_transformableExpressions[loop_var_reg]));
    
    append_to_current_block(std::move(declaration));
    m_transformableExpressions[loop_var_reg] = id->clone();
    m_transformableExpressions[loop_var_reg]->set_type(make_type(ast::primitive_kind::U32));
    m_registersToVars[loop_var_reg].push(id->copy());

    parse_basic_block(head_node);

    expr_uptr condition = m_transformableExpressions[loop_alternative_reg]->clone();

    for (const auto reg : regs_to_emit) {
        m_registersToVars[reg].push(std::make_unique<ast::identifier>(get_next_var()));
        regs_to_type.emplace(reg, std::monostate());
    }

    m_blockStack.push(*loop_block);
    emit_node(m_graph[loop_entry], head_node.m_startLine);
    for (const auto reg : regs_to_emit) {
        auto new_var = std::unique_ptr<ast::identifier>{static_cast<ast::identifier*>(m_registersToVars[reg].top()->clone().release())};
        load_expression_into_existing_var(reg, std::move(new_var), std::move(m_transformableExpressions[reg]));
        if (!is_unknown(m_transformableExpressions[reg]->get_type(m_env))) {
            regs_to_type[reg] = m_transformableExpressions[reg]->get_type(m_env);
        }
    }
    load_expression_into_existing_var(loop_var_reg, id->copy(), std::move(m_transformableExpressions[loop_var_reg]));
    m_blockStack.pop();

    for (const auto reg : regs_to_emit) {
        const auto &type = regs_to_type[reg];
        m_transformableExpressions[reg] = m_registersToVars[reg].top()->clone();
        m_transformableExpressions[reg]->set_type(type);
        const std::string type_name = type_to_declaration_string(type);
        append_to_current_block(std::make_unique<ast::variable_declaration>(type_name, m_registersToVars[reg].top()->m_name.m_lexeme));
        m_registersToVars[reg].pop();
    }

    auto full_while = std::make_unique<ast::while_stmt>(std::move(condition), std::move(loop_block));
    append_to_current_block(std::move(full_while));

    emit_node(m_graph[loop_tail], stop_node);
}

void decomp_function::emit_branches(const control_flow_node &node, node_id stop_node) {
    const node_id idom = m_graph.get_immediate_postdominators().at(node.m_startLine);
    const b8 idom_already_emitted = m_graph.m_ipdomsEmitted.contains(idom);
    m_graph.m_ipdomsEmitted.insert(idom);
    node_id proper_successor, proper_destination, proper_head;
    expr_uptr condition = make_condition(node, proper_head, proper_successor, proper_destination);
    auto then_block = std::make_unique<ast::block>();
    auto else_block = std::make_unique<ast::block>();
    std::set<reg_idx> regs_to_emit = m_graph.get_branch_variant_registers(proper_head);
    std::unordered_map<reg_idx, ast::full_type> regs_to_type;

    if (!idom_already_emitted) {
        for (const auto reg : regs_to_emit) {
            m_registersToVars[reg].push(std::make_unique<ast::identifier>(get_next_var()));
            regs_to_type.emplace(reg, std::monostate());
        }
    }

    emit_branch(*then_block, proper_successor, idom, regs_to_emit, regs_to_type);

    emit_branch(*else_block, proper_destination, idom, regs_to_emit, regs_to_type);

    for (const auto reg : regs_to_emit) {
        if (!idom_already_emitted) {
            const auto &type = regs_to_type[reg];
            m_transformableExpressions[reg] = m_registersToVars[reg].top()->clone();
            m_transformableExpressions[reg]->set_type(type);
            const std::string type_name = type_to_declaration_string(type);
            append_to_current_block(std::make_unique<ast::variable_declaration>(type_name, m_registersToVars[reg].top()->m_name.m_lexeme));
            m_registersToVars[reg].pop();
        }
    }

    stmnt_uptr _else = nullptr;
    if (else_block->m_statements.size() == 1 && dynamic_cast<ast::if_stmt*>(else_block->m_statements[0].get())) {
        _else = std::move(else_block->m_statements[0]);
    }
    else if (!else_block->m_statements.empty()) {
        _else = std::move(else_block);
    }

    stmnt_uptr full_if = std::make_unique<ast::if_stmt>(std::move(condition), std::move(then_block), std::move(_else));

    append_to_current_block(std::move(full_if));

    emit_node(m_graph[idom], stop_node);
}

void decomp_function::emit_branch(ast::block &else_block, const node_id target, const node_id idom, const std::set<reg_idx> &regs_to_emit, std::unordered_map<reg_idx, ast::full_type> &regs_to_type) {
    m_blockStack.push(else_block);
    emit_node(m_graph[target], idom);
    for (const auto reg : regs_to_emit) {
        load_expression_into_existing_var(reg, m_registersToVars[reg].top()->copy(), std::move(m_transformableExpressions[reg]));
        if (!is_unknown(m_transformableExpressions[reg]->get_type(m_env))) {
            regs_to_type[reg] = m_transformableExpressions[reg]->get_type(m_env);
        }
    }
    m_blockStack.pop();
}


void decomp_function::load_expression_into_new_var(const reg_idx dst, expr_uptr&& expr) {
    auto id = std::make_unique<ast::identifier>(compiler::token{ compiler::token_type::IDENTIFIER, get_next_var()});
    const std::string name = id->m_name.m_lexeme;
    const ast::full_type type = expr->get_type(m_env);
    const std::string type_name = type_to_declaration_string(type);

    auto var_declaration = std::make_unique<ast::variable_declaration>(type_name, name, std::move(expr));

    append_to_current_block(std::move(var_declaration));
    m_transformableExpressions[dst] = std::move(id);
    m_transformableExpressions[dst]->set_type(type);
}

void decomp_function::load_expression_into_existing_var(const reg_idx dst, std::unique_ptr<ast::identifier>&& var, expr_uptr&& expr) {
    auto rhs_ptr = dynamic_cast<ast::identifier*>(expr.get());
    if (rhs_ptr && rhs_ptr->m_name == var->m_name) {
        return;
    }
    const auto type_temp = expr->get_type(m_env);
    auto assign_expr = std::make_unique<ast::assign_expr>(compiler::token{ compiler::token_type::IDENTIFIER, var->m_name.m_lexeme }, std::move(expr));

    auto assign_statement = std::make_unique<ast::expression_stmt>(std::move(assign_expr));

    append_to_current_block(std::move(assign_statement));
    m_transformableExpressions[dst] = std::move(var);
    m_transformableExpressions[dst]->set_type(type_temp);
}

[[nodiscard]] expr_uptr decomp_function::make_call(const Instruction& istr) {
    expr_uptr callee = m_transformableExpressions[istr.destination]->clone();  
    std::vector<expr_uptr> args;
    ast::function_type func_type = std::get<ast::function_type>(callee->get_type(m_env));

    for (reg_idx i = 0; i < istr.operand2; ++i) {
        if (m_transformableExpressions[ARGUMENT_REGISTERS_IDX + i]->complexity() > max_expression_complexity) {
            load_expression_into_new_var(ARGUMENT_REGISTERS_IDX + i, std::move(m_transformableExpressions[ARGUMENT_REGISTERS_IDX + i]));
        }
        args.push_back(m_transformableExpressions[ARGUMENT_REGISTERS_IDX + i]->clone());
        args[i]->set_type(*func_type.m_arguments[i].second);
    }

    auto expr = std::make_unique<ast::call_expr>(compiler::token{ compiler::token_type::_EOF, "" }, std::move(callee), std::move(args));;
    expr->set_type(*func_type.m_return);
    return expr;
}


[[nodiscard]] expr_uptr decomp_function::make_cast_to_int(const Instruction& istr) {
    const ast::literal* old_lit = dynamic_cast<ast::literal*>(m_transformableExpressions[istr.operand1].get());
    if (old_lit == nullptr) {
        const auto& op2 = m_transformableExpressions[istr.operand1];
        return std::make_unique<ast::cast_expr>(
            compiler::token{ compiler::token_type::IDENTIFIER, "i32"}, 
            std::make_unique<ast::identifier>("i32"), 
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
            compiler::token{ compiler::token_type::IDENTIFIER, "f32"}, 
            std::make_unique<ast::identifier>("f32"), 
            is_binary(op2.get()) ? std::make_unique<ast::grouping>(op2->clone()) : op2->clone()
        );
    }
    else {
        return std::make_unique<ast::literal>(static_cast<f32>(std::get<i32>(old_lit->m_value)));
    }
}

[[nodiscard]] expr_uptr decomp_function::make_condition(const control_flow_node& condition_start, node_id& proper_head, node_id& proper_successor, node_id& proper_destination) {
    const auto and_token = compiler::token{ compiler::token_type::AMPERSAND_AMPERSAND, "&&" };
    const auto or_token = compiler::token{ compiler::token_type::PIPE_PIPE, "||" };
    const control_flow_node* current_node = &condition_start;
    expr_uptr condition = m_transformableExpressions[current_node->get_last_line().m_instruction.operand1]->clone();
    proper_head = current_node->m_startLine;
    proper_successor = current_node->get_direct_successor();
    proper_destination = current_node->get_last_line().m_target;
    while (true) {
        const auto& last_line = current_node->get_last_line();
        const u16 unlinked_target = last_line.m_instruction.destination | (last_line.m_instruction.operand2 << 8);
        const auto& successor = m_graph[proper_successor];
        const b8 linked = unlinked_target == successor.m_endLine;
        if (!linked) {
            break;
        }
        parse_basic_block(successor);
        if (last_line.m_instruction.opcode == Opcode::BranchIfNot) {
            condition = std::make_unique<ast::logical_expr>(and_token, std::move(condition), m_transformableExpressions[current_node->get_last_line().m_instruction.operand1]->clone());
            proper_head = proper_successor;
            proper_successor = successor.get_direct_successor();
            proper_destination = successor.get_last_line().m_instruction.destination | (successor.get_last_line().m_instruction.operand2 << 8);
        } else if (last_line.m_instruction.opcode == Opcode::BranchIf) {
            condition = std::make_unique<ast::logical_expr>(or_token, std::move(condition), m_transformableExpressions[current_node->get_last_line().m_instruction.operand1]->clone());
            proper_head = proper_successor;
            proper_successor = successor.get_direct_successor();
            proper_destination = successor.get_last_line().m_instruction.destination | (successor.get_last_line().m_instruction.operand2 << 8);
        }
        current_node = &m_graph[current_node->get_direct_successor()];
    }
    return condition;
}

[[nodiscard]] expr_uptr decomp_function::make_dereference(const Instruction& istr) {
    expr_uptr load = std::make_unique<ast::dereference_expr>(compiler::token{ compiler::token_type::STAR, "*" }, std::move(m_transformableExpressions[istr.operand1]));
    return load;
}

void decomp_function::insert_return(const reg_idx dest) {
    m_returnType = m_transformableExpressions[dest]->get_type(m_env);
    append_to_current_block(std::make_unique<ast::return_stmt>(std::move(m_transformableExpressions[dest])));
}



}