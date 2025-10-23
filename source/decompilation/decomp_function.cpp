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
        m_transformableExpressions.emplace_back(std::make_unique<ast::identifier>(compiler::token{ compiler::token_type::IDENTIFIER, "arg_" + std::to_string(i - ARGUMENT_REGISTERS_IDX)}));
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

void decomp_function::parse_basic_block(const control_flow_node &node) {
    std::cout << "parsing node " << node.m_startLine << '\n';


    for (const auto &line : node.m_lines) {
        const Instruction &istr = line.m_instruction;

        expr_uptr generated_expression = nullptr;

        switch(istr.opcode) {
            case Opcode::IAdd:
            case Opcode::FAdd: generated_expression = apply_binary_op<ast::add_expr>(istr); break;
            case Opcode::ISub:
            case Opcode::FSub: generated_expression = apply_binary_op<ast::sub_expr>(istr); break;
            case Opcode::IMul:
            case Opcode::FMul: generated_expression = apply_binary_op<ast::mul_expr>(istr); break;
            case Opcode::IDiv:
            case Opcode::FDiv: generated_expression = apply_binary_op<ast::div_expr>(istr); break;

            case Opcode::IAddImm: generated_expression = apply_binary_op_imm<ast::add_expr>(istr); break;
            case Opcode::IMulImm: generated_expression = apply_binary_op_imm<ast::mul_expr>(istr); break;
            case Opcode::ISubImm: generated_expression = apply_binary_op_imm<ast::sub_expr>(istr); break;
            case Opcode::IDivImm: generated_expression = apply_binary_op_imm<ast::div_expr>(istr); break;

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

            case Opcode::OpLogNot: generated_expression = apply_unary_op<ast::logical_not_expr>(istr); break;
            case Opcode::OpBitNot: generated_expression = apply_unary_op<ast::bitwise_not_expr>(istr); break;
            case Opcode::INeg:
            case Opcode::FNeg: generated_expression = apply_unary_op<ast::negate_expr>(istr); break;

            case Opcode::LoadU16Imm: generated_expression = std::make_unique<ast::literal>(static_cast<u16>(istr.operand1 | static_cast<u16>(istr.operand2) << 8)); break;
            case Opcode::LoadStaticInt: generated_expression = std::make_unique<ast::literal>(m_symbolTable.first.get<i32>()); break;
            case Opcode::LoadStaticFloat: generated_expression = std::make_unique<ast::literal>(m_symbolTable.first.get<f32>()); break;
            case Opcode::LoadStaticI32Imm: generated_expression = std::make_unique<ast::literal>(m_symbolTable.first.get<i32>(istr.operand1 * 8)); break;

            case Opcode::Call:
            case Opcode::CallFf: {
                expr_uptr call = make_call(istr);
                std::set<node_id> checked;
                u16 call_usage_count = m_graph.get_register_read_count(node.m_startLine, istr.destination, m_graph.get_return_node().m_startLine, checked, line.m_location - node.m_startLine + 1);
                if (call_usage_count == 0) {
                    append_to_current_block(std::make_unique<ast::expression_stmt>(std::move(call)));
                } else if (call_usage_count == 1) {
                    generated_expression = std::move(call);
                } else {
                    m_transformableExpressions[istr.destination] = std::move(call);
                    load_expression_into_new_var(istr.destination);
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
                    generated_expression->set_type(m_symbolTable.second[istr.operand1]);
                }
                else {
                    m_transformableExpressions[istr.destination] = std::move(lit);
                    m_transformableExpressions[istr.destination]->set_type(m_symbolTable.second[istr.operand1]);
                }
                break;
            }

            case Opcode::CastInteger: {
                generated_expression = make_cast<f32, i32>(istr, make_type(ast::primitive_kind::I32));
                break;
            }
            case Opcode::CastFloat: {
                generated_expression = make_cast<i32, f32>(istr, make_type(ast::primitive_kind::F32));
                break;
            }

            case Opcode::Move: {
                generated_expression = m_transformableExpressions[istr.operand1]->clone();
                break;
            }

            case Opcode::LoadFloat: {
                generated_expression = std::make_unique<ast::dereference_expr>(make_cast<u64, u64>(istr, ast::ptr_type{ ast::primitive_kind::F32 })); break;
            }
            case Opcode::LoadI32: {
                generated_expression = std::make_unique<ast::dereference_expr>(make_cast<u64, u64>(istr, ast::ptr_type{ ast::primitive_kind::I32 })); break;
            }
            case Opcode::LoadU64: {
                generated_expression = std::make_unique<ast::dereference_expr>(make_cast<u64, u64>(istr, ast::ptr_type{ ast::primitive_kind::U64 })); break;
            }
            case Opcode::LoadU8: {
                generated_expression = std::make_unique<ast::dereference_expr>(make_cast<u64, u64>(istr, ast::ptr_type{ ast::primitive_kind::U8 })); break;
            }
            case Opcode::LoadU32: {
                generated_expression = std::make_unique<ast::dereference_expr>(make_cast<u64, u64>(istr, ast::ptr_type{ ast::primitive_kind::U32 })); break;
            }
            case Opcode::LoadU16: {
                generated_expression = std::make_unique<ast::dereference_expr>(make_cast<u64, u64>(istr, ast::ptr_type{ ast::primitive_kind::U16 })); break;
            }
            case Opcode::LoadI64: {
                generated_expression = std::make_unique<ast::dereference_expr>(make_cast<u64, u64>(istr, ast::ptr_type{ ast::primitive_kind::I64 })); break;
            }
            case Opcode::LoadPointer: {
                generated_expression = std::make_unique<ast::dereference_expr>(make_cast<u64, u64>(istr, ast::ptr_type{ ast::ptr_type {} })); break;
            }

            case Opcode::AssertPointer: {
                m_transformableExpressions[istr.destination]->set_type(ast::ptr_type{});
                break;
            }
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
        const node_id idom = m_graph.get_ipdom_at(node.m_startLine);
        if (idom == node.get_target()) {
            emit_single_branch(node, stop_node);
        } else {
            emit_branches(node, stop_node);
        }
    }
}

void decomp_function::emit_single_branch(const control_flow_node& node, const node_id stop_node) {
    const node_id idom = m_graph.get_ipdom_at(node.m_startLine);
    const b8 idom_already_emitted = m_graph.m_ipdomsEmitted.contains(idom);
    m_graph.m_ipdomsEmitted.insert(idom);
    node_id proper_successor, proper_destination, proper_head;
    expr_uptr condition = make_condition(node, proper_head, proper_successor, proper_destination);
    auto then_block = std::make_unique<ast::block>();
    std::set<reg_idx> regs_to_emit = m_graph.get_branch_phi_registers(m_graph[proper_head]);
    std::unordered_map<reg_idx, ast::full_type> regs_to_type;
    std::unordered_map<reg_idx, expr_uptr> regs_to_potential_inits;

    for (const auto reg : regs_to_emit) {
        m_registersToVars[reg].push(std::make_unique<ast::identifier>(get_next_var()));
        regs_to_type.emplace(reg, std::monostate());
        if (reg == m_graph[proper_head].get_last_line().m_instruction.operand1) {
            regs_to_potential_inits[reg] = std::move(condition);
            regs_to_type[reg] = make_type(ast::primitive_kind::BOOL);
            condition = m_registersToVars[reg].top()->clone();
        }
        else {
            regs_to_potential_inits[reg] = m_transformableExpressions[reg]->clone();
        }
    }

    m_blockStack.push(*then_block);
    emit_node(m_graph[proper_successor], idom);
    for (const auto reg : regs_to_emit) {
        load_expression_into_existing_var(reg, m_registersToVars[reg].top()->copy());
        if (!is_unknown(m_transformableExpressions[reg]->get_type(m_env))) {
            regs_to_type[reg] = m_transformableExpressions[reg]->get_type(m_env);
        }
    }
    m_blockStack.pop();

    for (const auto reg : regs_to_emit) {
        const auto &type = regs_to_type[reg];
        m_transformableExpressions[reg] = m_registersToVars[reg].top()->clone();
        if (!is_unknown(type)) {
            m_transformableExpressions[reg]->set_type(type);
        }
        const std::string type_name = type_to_declaration_string(m_transformableExpressions[reg]->get_type(m_env));
        append_to_current_block(std::make_unique<ast::variable_declaration>(type_name, m_registersToVars[reg].top()->m_name.m_lexeme, std::move(regs_to_potential_inits[reg])));
        m_registersToVars[reg].pop();
    }

    append_to_current_block(std::make_unique<ast::if_stmt>(std::move(condition), std::move(then_block)));

    emit_node(m_graph[idom], stop_node);
}

[[nodiscard]] b8 decomp_function::is_for_loop(const control_flow_loop& loop) const noexcept {
    constexpr Opcode standard_for_loop_pattern[] = {
        Opcode::Move,
        Opcode::Move,
        Opcode::ILessThan,
        Opcode::BranchIfNot,
    };
    const node_id idom = m_graph.get_ipdom_at(loop.m_headNode);
    if (idom != loop.m_lastConditionalNode) {
        return false;
    }
    const auto& lines = m_graph[loop.m_headNode].m_lines;
    for (u32 i = 0; i < 4; ++i) {
        if (lines[i].m_instruction.opcode != standard_for_loop_pattern[i]) {
            return false;
        }
    }
}

void decomp_function::emit_loop(const function_disassembly_line &detect_node_last_line, const control_flow_loop &loop, const node_id stop_node) {
    if (is_for_loop(loop)) {
        emit_for_loop();
    } else {
        emit_while_loop();
    }


    // const control_flow_node& head_node = m_graph[detect_node_last_line.m_location + 1];
    // const node_id loop_entry = head_node.get_direct_successor();
    // const node_id loop_tail = head_node.get_target();
    // auto loop_block = std::make_unique<ast::block>();
    // std::set<reg_idx> regs_to_emit = m_graph.get_loop_phi_registers(head_node);
    // const node_id idom = m_graph.get_ipdom_at(loop_tail);
    // std::unordered_map<reg_idx, ast::full_type> regs_to_type;

    // reg_idx loop_var_reg = head_node.m_lines[0].m_instruction.operand1;
    // reg_idx loop_alternative_reg = head_node.get_last_line().m_instruction.operand1;
    // regs_to_emit.erase(loop_var_reg);
    // regs_to_emit.erase(loop_alternative_reg);
    // const std::string name = "i";
    // auto id = std::make_unique<ast::identifier>(name);

    // auto declaration = std::make_unique<ast::variable_declaration>("u32", name, std::move(m_transformableExpressions[loop_var_reg]));

    // append_to_current_block(std::move(declaration));
    // m_transformableExpressions[loop_var_reg] = id->clone();
    // m_transformableExpressions[loop_var_reg]->set_type(make_type(ast::primitive_kind::U32));
    // m_registersToVars[loop_var_reg].push(id->copy());

    // parse_basic_block(head_node);

    // expr_uptr condition = m_transformableExpressions[loop_alternative_reg]->clone();

    // for (const auto reg : regs_to_emit) {
    //     m_registersToVars[reg].push(std::make_unique<ast::identifier>(get_next_var()));
    //     regs_to_type.emplace(reg, std::monostate());
    // }

    // m_blockStack.push(*loop_block);
    // emit_node(m_graph[loop_entry], head_node.m_startLine);
    // for (const auto reg : regs_to_emit) {
    //     auto new_var = std::unique_ptr<ast::identifier>{static_cast<ast::identifier*>(m_registersToVars[reg].top()->clone().release())};
    //     load_expression_into_existing_var(reg, std::move(new_var));
    //     if (!is_unknown(m_transformableExpressions[reg]->get_type(m_env))) {
    //         regs_to_type[reg] = m_transformableExpressions[reg]->get_type(m_env);
    //     }
    // }
    // load_expression_into_existing_var(loop_var_reg, id->copy());
    // m_blockStack.pop();

    // for (const auto reg : regs_to_emit) {
    //     const auto &type = regs_to_type[reg];
    //     m_transformableExpressions[reg] = m_registersToVars[reg].top()->clone();
    //     m_transformableExpressions[reg]->set_type(type);
    //     const std::string type_name = type_to_declaration_string(type);
    //     append_to_current_block(std::make_unique<ast::variable_declaration>(type_name, m_registersToVars[reg].top()->m_name.m_lexeme));
    //     m_registersToVars[reg].pop();
    // }

    // auto full_while = std::make_unique<ast::while_stmt>(std::move(condition), std::move(loop_block));
    // append_to_current_block(std::move(full_while));


    const node_id loop_tail = m_graph[loop.m_headNode].get_target();
    auto loop_block = std::make_unique<ast::block>();

    const node_id proper_loop_head = m_graph.get_proper_loop_head(loop.m_headNode, loop.m_latchNode);
    const node_id idom = m_graph.get_ipdom_at(loop_tail);

    node_id proper_head, proper_destination, proper_successor;
    const auto condition = make_condition(m_graph[loop.m_headNode], proper_head, proper_successor, proper_destination);

    std::set<reg_idx> regs_to_emit = m_graph.get_loop_phi_registers(m_graph[proper_head]);

    auto id = std::make_unique<ast::identifier>("i");

    auto declaration = std::make_unique<ast::variable_declaration>("u32", "i", std::move(m_transformableExpressions[m_graph[proper_head].get_last_line().m_instruction.destination]));

    emit_node(m_graph[loop_tail], stop_node);
}

void decomp_function::emit_branches(const control_flow_node &node, node_id stop_node) {
    const node_id idom = m_graph.get_ipdom_at(node.m_startLine);
    const b8 idom_already_emitted = m_graph.m_ipdomsEmitted.contains(idom);
    m_graph.m_ipdomsEmitted.insert(idom);
    node_id proper_successor, proper_destination, proper_head;
    expr_uptr condition = make_condition(node, proper_head, proper_successor, proper_destination);
    auto then_block = std::make_unique<ast::block>();
    auto else_block = std::make_unique<ast::block>();
    std::set<reg_idx> regs_to_emit = m_graph.get_branch_phi_registers(m_graph[proper_head]);
    std::unordered_map<reg_idx, ast::full_type> regs_to_type;

    if (!idom_already_emitted) {
        for (const auto reg : regs_to_emit) {
            m_registersToVars[reg].push(std::make_unique<ast::identifier>(get_next_var()));
            regs_to_type.emplace(reg, std::monostate());
        }
    }

    emit_branch(*then_block, proper_successor, idom, regs_to_emit, regs_to_type);

    emit_branch(*else_block, proper_destination, idom, regs_to_emit, regs_to_type);

    if (!idom_already_emitted) {
        for (const auto reg : regs_to_emit) {
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
        load_expression_into_existing_var(reg, m_registersToVars[reg].top()->copy());
        if (!is_unknown(m_transformableExpressions[reg]->get_type(m_env))) {
            regs_to_type[reg] = m_transformableExpressions[reg]->get_type(m_env);
        }
    }
    m_blockStack.pop();
}


void decomp_function::load_expression_into_new_var(const reg_idx dst) {
    auto& expr = m_transformableExpressions[dst];
    auto id = std::make_unique<ast::identifier>(compiler::token{ compiler::token_type::IDENTIFIER, get_next_var()});
    const std::string name = id->m_name.m_lexeme;
    const ast::full_type type = expr->get_type(m_env);
    const std::string type_name = type_to_declaration_string(type);

    auto var_declaration = std::make_unique<ast::variable_declaration>(type_name, name, std::move(expr));

    append_to_current_block(std::move(var_declaration));
    m_registersToVars[dst].push(id->copy());
    m_transformableExpressions[dst] = std::move(id);
    m_transformableExpressions[dst]->set_type(type);
}

void decomp_function::load_expression_into_existing_var(const reg_idx dst, std::unique_ptr<ast::identifier>&& var) {
    auto& expr = m_transformableExpressions[dst];
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
        if (m_transformableExpressions[ARGUMENT_REGISTERS_IDX + i]->complexity() > MAX_EXPRESSION_COMPLEXITY) {
            load_expression_into_new_var(ARGUMENT_REGISTERS_IDX + i);
        }
        args.push_back(m_transformableExpressions[ARGUMENT_REGISTERS_IDX + i]->clone());
        args[i]->set_type(*func_type.m_arguments[i].second);
    }

    auto expr = std::make_unique<ast::call_expr>(compiler::token{ compiler::token_type::_EOF, "" }, std::move(callee), std::move(args));;
    expr->set_type(*func_type.m_return);
    return expr;
}

template<typename from, typename to>
[[nodiscard]] expr_uptr decomp_function::make_cast(const Instruction& istr, const ast::full_type& type) {
    const ast::literal* old_lit = dynamic_cast<ast::literal*>(m_transformableExpressions[istr.operand1].get());
    if (old_lit == nullptr) {
        const auto& op2 = m_transformableExpressions[istr.operand1];
        const std::string type_name = ast::type_to_declaration_string(type);
        auto cast = std::make_unique<ast::cast_expr>(
            type_name,
            is_binary(op2.get()) ? std::make_unique<ast::grouping>(op2->clone()) : op2->clone()
        );
        cast->set_type(type);
        return cast;
    }
    else {
        return std::make_unique<ast::literal>(static_cast<to>(std::get<from>(old_lit->m_value)));
    }
}

[[nodiscard]] expr_uptr decomp_function::get_expression_as_condition(const reg_idx src) const noexcept {
    expr_uptr condition = m_transformableExpressions[src]->clone();
    if (std::holds_alternative<ast::ptr_type>(condition->get_type(m_env)) || std::holds_alternative<ast::function_type>(condition->get_type(m_env))) {
        condition = std::make_unique<ast::compare_expr>(compiler::token{ compiler::token_type::BANG_EQUAL, "!=" }, std::move(condition), std::make_unique<ast::literal>(nullptr));
    }
    return condition;
}

[[nodiscard]] expr_uptr decomp_function::make_condition(
    const control_flow_node& condition_start,
    node_id& proper_head,
    node_id& proper_successor,
    node_id& proper_destination
) {
    const auto and_token = compiler::token{ compiler::token_type::AMPERSAND_AMPERSAND, "&&" };
    const auto or_token = compiler::token{ compiler::token_type::PIPE_PIPE, "||" };

    const control_flow_node* current_node = &condition_start;
    const control_flow_node* failure_exit = nullptr;
    const control_flow_node* success_exit = nullptr;

    expr_uptr condition = get_expression_as_condition(current_node->get_last_line().m_instruction.operand1);
    proper_head = current_node->m_startLine;
    proper_successor = current_node->get_direct_successor();
    proper_destination = current_node->get_last_line().m_target;

    while (true) {
        const auto& last_line = current_node->get_last_line();
        const auto  target = last_line.m_target;

        const b8 is_and = last_line.m_instruction.opcode == Opcode::BranchIfNot;
        const b8 is_or = last_line.m_instruction.opcode == Opcode::BranchIf;

        if (!is_and && !is_or)
            break;

        auto& exit_node = is_and ? failure_exit : success_exit;
        auto& alt_exit = is_and ? success_exit : failure_exit;
        const auto& token = is_and ? and_token : or_token;

        if (exit_node == nullptr) {
            if (alt_exit == nullptr) {
                exit_node = &m_graph[target];
            }
            else {
                parse_basic_block(*current_node);
                condition = std::make_unique<ast::logical_expr>(
                    token, std::move(condition),
                    get_expression_as_condition(last_line.m_instruction.operand1)
                );
                proper_head = current_node->m_startLine;
                proper_successor = current_node->get_direct_successor();
                proper_destination = target;
            }
        }
        else if (exit_node->m_startLine != target) {
            break;
        }
        else {
            parse_basic_block(*current_node);
            condition = std::make_unique<ast::logical_expr>(
                token, std::move(condition),
                get_expression_as_condition(last_line.m_instruction.operand1)
            );
            proper_head = current_node->m_startLine;
            proper_successor = current_node->get_direct_successor();
            proper_destination = target;
        }

        current_node = &m_graph[current_node->get_direct_successor()];
    }

    return condition;
}

void decomp_function::insert_return(const reg_idx dest) {
    m_returnType = m_transformableExpressions[dest]->get_type(m_env);
    append_to_current_block(std::make_unique<ast::return_stmt>(std::move(m_transformableExpressions[dest])));
}



}
