#include "decompilation/decomp_function.h"
#include <sstream>
#include <iostream>

namespace dconstruct::dcompiler {


static const auto and_token = compiler::token{ compiler::token_type::AMPERSAND_AMPERSAND, "&&" };
static const auto or_token = compiler::token{ compiler::token_type::PIPE_PIPE, "||" };

decomp_function::decomp_function(const function_disassembly *func, const BinaryFile &current_file) :
    m_disassembly{func}, 
    m_file{current_file}, 
    m_graph{func}, 
    m_symbolTable{func->m_stackFrame.m_symbolTable}, 
    m_parsedNodes(m_graph.m_nodes.size(), false), 
    m_ipdomsEmitted(m_graph.m_nodes.size(), false)
{
#ifdef _DEBUG
    std::cout << "parsing graph " << m_disassembly->m_id << '\n';
    if (m_graph.m_nodes.size() > 1) {
        m_graph.write_image(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\images\)" + m_disassembly->m_id + ".svg");
    }
#endif

    m_blockStack.push(std::ref(m_baseBlock));
    m_transformableExpressions.resize(ARGUMENT_REGISTERS_IDX);
    for (reg_idx i = 0; i < ARGUMENT_REGISTERS_IDX; ++i) {
        m_registersToVars.emplace(i, std::stack<std::unique_ptr<ast::identifier>>());
    }
    for (reg_idx i = ARGUMENT_REGISTERS_IDX; i < 128; ++i) {
        m_transformableExpressions.emplace_back(std::make_unique<ast::identifier>(compiler::token{ compiler::token_type::IDENTIFIER, "arg_" + std::to_string(i - ARGUMENT_REGISTERS_IDX)}));
        m_registersToVars.emplace(i, std::stack<std::unique_ptr<ast::identifier>>());
    }

    for (reg_idx i = 0; i < func->m_stackFrame.m_registerArgs.size(); ++i) {
        const auto& arg_type = func->m_stackFrame.m_registerArgs.at(i);
        m_arguments.push_back(ast::variable_declaration(arg_type, "arg_" + std::to_string(i)));
    }

    emit_node(m_graph[0], m_graph.m_nodes.back().m_index);
    parse_basic_block(m_graph.m_nodes.back());
}

[[nodiscard]] std::string decomp_function::to_string() const {
    std::ostringstream out;
    const std::string return_type = m_disassembly->m_isScriptFunction ? "void" : ast::type_to_declaration_string(m_returnType);
    out << return_type << ' ' << m_disassembly->m_id << '(';
    for (u8 i = 0; i < m_arguments.size(); ++i) {
        out << type_to_declaration_string(m_arguments[i].m_type) << ' ' << m_arguments[i].m_identifier;
        if (i != m_arguments.size() - 1) {
            out << ", ";
        }
    }
    out << ") " << m_baseBlock;
    return out.str();
}

void decomp_function::parse_basic_block(const control_flow_node &node) {
#ifdef _DEBUG
    std::cout << "parsing block " << std::hex << node.m_startLine << std::dec << " (" << node.m_index << ")\n";
#endif


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
                node_set checked(m_graph.m_nodes.size(), false);
                u16 call_usage_count = m_graph.get_register_read_count(node, istr.destination, m_graph.m_nodes.back().m_index, checked, !m_disassembly->m_isScriptFunction, line.m_location - node.m_startLine + 1);
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
                if (m_symbolTable.first.get<p64>(istr.operand1 * 8) >= (p64)(m_file.m_strings.m_ptr)) {
                    generated_expression = std::make_unique<ast::literal>(m_symbolTable.first.get<const char*>(istr.operand1 * 8));
                } else {
                    generated_expression = std::make_unique<ast::literal>("");
                }
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
                generated_expression = apply_unary_op<ast::dereference_expr>(istr); break;
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

            case Opcode::Return:{
                if (!m_disassembly->m_isScriptFunction) {
                    insert_return(istr.destination);
                }
                break;
            }
            default: {
				std::cerr << "unhandled opcode " << static_cast<u64>(istr.opcode) << " at " << std::hex << line.m_location << std::dec << '\n';
                std::terminate();
            }
        }
        if (generated_expression != nullptr) {
            m_transformableExpressions[istr.destination] = std::move(generated_expression);
        }
    }
}

void decomp_function::emit_node(const control_flow_node& node, const node_id stop_node) {
#ifdef _DEBUG
    std::cout << "emitting node " << std::hex << node.m_startLine << std::dec << " (" << node.m_index << ")\n";
#endif

    const node_id current_node_id = node.m_index;
    if (current_node_id == stop_node || m_parsedNodes[current_node_id]) {
        return;
    }
    m_parsedNodes[current_node_id] = true;

    const auto& last_line = node.m_lines.back();

    if (const auto loop = m_graph.get_loop_with_head(node.m_index)) {
        emit_loop(loop->get(), stop_node);
    }
    else if (last_line.m_instruction.opcode == Opcode::BranchIf || last_line.m_instruction.opcode == Opcode::BranchIfNot) {
        parse_basic_block(node);
        if (node.m_ipdom == node.m_targetSuccessor) {
            emit_single_branch(node, stop_node);
        } else {
            emit_branches(node, stop_node);
        }
    }
    else if (const auto loop = m_graph.get_loop_with_head(node.m_directSuccessor)) {
        if (node.m_directSuccessor != stop_node) {
            parse_basic_block(node);
            emit_loop(loop->get(), stop_node);
        }
    }
    else {
        parse_basic_block(node);
    }
}

void decomp_function::emit_single_branch(const control_flow_node& node, const node_id stop_node) {
    const node_id idom = node.m_ipdom;
    const bool idom_already_emitted = m_ipdomsEmitted[idom];
    m_ipdomsEmitted[idom] = true;

    const reg_idx check_register = node.m_lines.back().m_instruction.operand1;

    const control_flow_node* target = &m_graph[node.m_targetSuccessor];

    const control_flow_node* current_node = &m_graph[node.m_directSuccessor];

    expr_uptr final_condition = m_transformableExpressions[check_register]->clone();

    while (current_node != target) {
        parse_basic_block(*current_node);
        const auto& token = current_node->m_lines.back().m_instruction.opcode == Opcode::BranchIf ? or_token : and_token;
        final_condition = std::make_unique<ast::compare_expr>(token, m_transformableExpressions[check_register]->clone(), std::move(final_condition));
        current_node = &m_graph[current_node->m_directSuccessor];
    }

    auto id = std::make_unique<ast::identifier>(get_next_var());

    auto declaration = std::make_unique<ast::variable_declaration>(make_type(ast::primitive_kind::BOOL), id->m_name.m_lexeme, std::move(final_condition));

    m_transformableExpressions[check_register] = std::move(id);
    append_to_current_block(std::move(declaration));


    emit_node(m_graph[idom], stop_node);
}

[[nodiscard]] bool decomp_function::is_for_loop(const control_flow_loop& loop) const noexcept {
    constexpr Opcode standard_for_loop_pattern[] = {
        Opcode::Move,
        Opcode::Move,
        Opcode::ILessThan,
        Opcode::BranchIfNot,
    };
    if (m_graph[loop.m_headNode].m_ipdom != loop.m_latchNode + 1) {
        return false;
    }
    const auto& lines = m_graph[loop.m_headNode].m_lines;
    for (u32 i = 0; i < sizeof(standard_for_loop_pattern); ++i) {
        if (lines[i].m_instruction.opcode != standard_for_loop_pattern[i]) {
            return false;
        }
    }
    return true;
}


void decomp_function::emit_for_loop(const control_flow_loop& loop, const node_id stop_node) {
    const control_flow_node& head_node = m_graph[loop.m_headNode];
    const node_id loop_entry = head_node.m_directSuccessor;
    const node_id loop_tail = head_node.m_targetSuccessor;
    auto loop_block = std::make_unique<ast::block>();
    reg_set regs_to_emit = m_graph.get_loop_phi_registers(head_node);
    const node_id idom = m_graph[loop_tail].m_ipdom;
    std::unordered_map<reg_idx, ast::full_type> regs_to_type;

    reg_idx loop_var_reg = head_node.m_lines[0].m_instruction.operand1;
    reg_idx loop_alternative_reg = head_node.m_lines.back().m_instruction.operand1;
    regs_to_emit.set(loop_var_reg, false);
    regs_to_emit.set(loop_alternative_reg, false);
    auto id = std::make_unique<ast::identifier>("i");

    auto declaration = std::make_unique<ast::variable_declaration>(make_type(ast::primitive_kind::U64), "i", std::move(m_transformableExpressions[loop_var_reg]));
    auto increment = std::make_unique<ast::increment_expression>(id->clone());

    m_transformableExpressions[loop_var_reg] = id->clone();
    m_transformableExpressions[loop_var_reg]->set_type(make_type(ast::primitive_kind::U64));
    m_registersToVars[loop_var_reg].push(id->copy());

    parse_basic_block(head_node);

    expr_uptr condition = m_transformableExpressions[loop_alternative_reg]->clone();

    auto bits = regs_to_emit.to_ullong();
    while (bits != 0) {
        const reg_idx reg = std::countr_zero(bits);
        bits &= bits - 1;
        m_registersToVars[reg].push(std::make_unique<ast::identifier>(get_next_var()));
        regs_to_type.emplace(reg, std::monostate());
    }

    m_blockStack.push(*loop_block);
    emit_node(m_graph[loop_entry], head_node.m_index);
    bits = regs_to_emit.to_ullong();
    while (bits != 0) {
        const reg_idx reg = std::countr_zero(bits);
        bits &= bits - 1;
        auto new_var = std::unique_ptr<ast::identifier>{ static_cast<ast::identifier*>(m_registersToVars[reg].top()->clone().release()) };
        load_expression_into_existing_var(reg, std::move(new_var));
        if (!is_unknown(m_transformableExpressions[reg]->get_type(m_env))) {
            regs_to_type[reg] = m_transformableExpressions[reg]->get_type(m_env);
        }
    }
    m_blockStack.pop();

    bits = regs_to_emit.to_ullong();
    while (bits != 0) {
        const reg_idx reg = std::countr_zero(bits);
        bits &= bits - 1;
        const auto& type = regs_to_type[reg];
        m_transformableExpressions[reg] = m_registersToVars[reg].top()->clone();
        m_transformableExpressions[reg]->set_type(type);
        append_to_current_block(std::make_unique<ast::variable_declaration>(type, m_registersToVars[reg].top()->m_name.m_lexeme));
        m_registersToVars[reg].pop();
    }

    auto for_loop = std::make_unique<ast::for_stmt>(std::move(declaration), std::move(condition), std::move(increment), std::move(loop_block));
    append_to_current_block(std::move(for_loop));
    emit_node(m_graph[loop_tail], stop_node);
}


[[nodiscard]] expr_uptr decomp_function::make_loop_condition(const node_id head_start, const node_id head_end, const node_id loop_entry, const node_id loop_exit) {

    expr_uptr condition = nullptr;
    for (u32 i = head_start; i != loop_entry; ++i) {
        const auto& current_node = m_graph[i];
        const auto& last_line = current_node.m_lines.back();

        const bool is_and = last_line.m_instruction.opcode == Opcode::BranchIfNot;
        const bool is_or = last_line.m_instruction.opcode == Opcode::BranchIf;

        parse_basic_block(current_node);
        auto current_expression = get_expression_as_condition(last_line.m_instruction.operand1);
        if (condition == nullptr) {
            condition = std::move(current_expression);
        } else {
            condition = std::make_unique<ast::logical_expr>(
                is_and ? and_token : or_token,
                std::move(condition),
                std::move(current_expression)
            );
        }
    }
    return condition;
}

void decomp_function::emit_while_loop(const control_flow_loop& loop, const node_id stop_node) {
    const node_id loop_tail = m_graph[loop.m_headNode].m_targetSuccessor;
    auto loop_block = std::make_unique<ast::block>();
    const node_id head_ipdom = m_graph[loop.m_headNode].m_ipdom; //m_graph.get_ipdom_at(loop.m_headNode);
    const node_id exit_node = loop.m_latchNode + 1;
    const node_id proper_loop_head = m_graph.get_final_loop_condition_node(loop, exit_node).m_index;
    const node_id idom = m_graph[loop_tail].m_ipdom; //m_graph.get_ipdom_at(loop_tail);

    const control_flow_node& loop_entry = m_graph[m_graph[proper_loop_head].m_directSuccessor];

    reg_set regs_to_emit = m_graph.get_loop_phi_registers(m_graph[proper_loop_head]);
    reg_idx alt_loop_var_reg = -1;
    std::unique_ptr<ast::identifier> id = nullptr;
    const Instruction& head_instruction = m_graph[loop.m_headNode].m_lines.front().m_instruction;
    const bool has_loop_variable = head_instruction.opcode == Opcode::Move;
    if (has_loop_variable) {
        const reg_idx loop_var_reg = head_instruction.operand1;
        alt_loop_var_reg = head_instruction.destination;
        regs_to_emit.reset(loop_var_reg);
        regs_to_emit.reset(alt_loop_var_reg);
        const std::string loop_var_name = std::string(1, m_loopVar++);
        id = std::make_unique<ast::identifier>(loop_var_name);
        auto declaration = std::make_unique<ast::variable_declaration>(make_type(ast::primitive_kind::U64), loop_var_name, std::move(m_transformableExpressions[loop_var_reg]));
        append_to_current_block(std::move(declaration));
        m_transformableExpressions[loop_var_reg] = id->copy();
        id->set_type(make_type(ast::primitive_kind::U64));
        m_registersToVars[loop_var_reg].push(id->copy());
        m_registersToVars[alt_loop_var_reg].push(id->copy());
    }
    expr_uptr condition = make_loop_condition(loop.m_headNode, proper_loop_head, loop_entry.m_index, exit_node);
    auto bits = regs_to_emit.to_ullong();
    while (bits != 0) {
        const reg_idx reg = std::countr_zero(bits);
        bits &= bits - 1;
        m_registersToVars[reg].push(std::make_unique<ast::identifier>(get_next_var()));
    }

    m_blockStack.push(*loop_block);
    emit_node(loop_entry, loop.m_headNode);
    bits = regs_to_emit.to_ullong();
    while (bits != 0) {
        const reg_idx reg = std::countr_zero(bits);
        bits &= bits - 1;
        auto new_var = m_registersToVars[reg].top()->copy();
        load_expression_into_existing_var(reg, std::move(new_var));
    }
    if (has_loop_variable) {
        load_expression_into_existing_var(alt_loop_var_reg, id->copy());
    }
    m_blockStack.pop();
    m_loopVar--;

    bits = regs_to_emit.to_ullong();
    while (bits != 0) {
        const reg_idx reg = std::countr_zero(bits);
        bits &= bits - 1;
        m_transformableExpressions[reg] = m_registersToVars[reg].top()->clone();
        append_to_current_block(std::make_unique<ast::variable_declaration>(std::monostate(), m_registersToVars[reg].top()->m_name.m_lexeme));
        m_registersToVars[reg].pop();
    }

    auto while_loop = std::make_unique<ast::while_stmt>(std::move(condition), std::move(loop_block));
    append_to_current_block(std::move(while_loop));

    emit_node(m_graph[exit_node], stop_node);
}

void decomp_function::emit_loop(const control_flow_loop &loop, const node_id stop_node) {
    if (is_for_loop(loop)) {
        emit_for_loop(loop, stop_node);
    } else {
        emit_while_loop(loop, stop_node);
    }
}


void decomp_function::emit_branches(const control_flow_node &node, node_id stop_node) {
    const node_id idom = node.m_ipdom;
    const bool idom_already_emitted = m_ipdomsEmitted[idom];
    m_ipdomsEmitted[idom] = true;
    node_id proper_successor, proper_destination, proper_head;
    expr_uptr condition = make_condition(node, proper_head, proper_successor, proper_destination);
    auto then_block = std::make_unique<ast::block>();
    auto else_block = std::make_unique<ast::block>();
    reg_set regs_to_emit = m_graph.get_branch_phi_registers(m_graph[proper_head], !m_disassembly->m_isScriptFunction);
    std::unordered_map<reg_idx, ast::full_type> regs_to_type;

    if (!idom_already_emitted) {
        auto bits = regs_to_emit.to_ullong();
        while (bits != 0) {
            const reg_idx reg = std::countr_zero(bits);
            bits &= bits - 1;
            m_registersToVars[reg].push(std::make_unique<ast::identifier>(get_next_var()));
            regs_to_type.emplace(reg, std::monostate());
        }
    }

    emit_branch(*then_block, proper_successor, idom, regs_to_emit, regs_to_type);

    emit_branch(*else_block, proper_destination, idom, regs_to_emit, regs_to_type);

    if (!idom_already_emitted) {
        auto bits = regs_to_emit.to_ullong();
        while (bits != 0) {
            const reg_idx reg = std::countr_zero(bits);
            bits &= bits - 1;
            const auto &type = regs_to_type[reg];
            m_transformableExpressions[reg] = m_registersToVars[reg].top()->clone();
            m_transformableExpressions[reg]->set_type(type);
            append_to_current_block(std::make_unique<ast::variable_declaration>(type, m_registersToVars[reg].top()->m_name.m_lexeme));
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

void decomp_function::emit_branch(ast::block &else_block, const node_id target, const node_id idom, reg_set regs_to_emit, std::unordered_map<reg_idx, ast::full_type> &regs_to_type) {
    m_blockStack.push(else_block);
    emit_node(m_graph[target], idom);
    auto bits = regs_to_emit.to_ullong();
    while (bits != 0) {
        const reg_idx reg = std::countr_zero(bits);
        bits &= bits - 1;
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

    auto var_declaration = std::make_unique<ast::variable_declaration>(type, name, std::move(expr));

    append_to_current_block(std::move(var_declaration));
    m_registersToVars[dst].push(id->copy());
    m_transformableExpressions[dst] = std::move(id);
    m_transformableExpressions[dst]->set_type(type);
}

void decomp_function::load_expression_into_existing_var(const reg_idx dst, std::unique_ptr<ast::identifier>&& var) {
    if (m_transformableExpressions[dst] == nullptr) {
        m_transformableExpressions[dst] = std::make_unique<ast::identifier>("upsi pupsi");
    }
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
        const ast::cast_expr* old_cast = dynamic_cast<ast::cast_expr*>(op2.get());
        if (old_cast != nullptr) {
            auto new_cast = std::make_unique<ast::cast_expr>(type, old_cast->m_rhs->clone());
            return new_cast;
        }
        else {
            auto cast = std::make_unique<ast::cast_expr>(
                type,
                is_binary(op2.get()) ? std::make_unique<ast::grouping>(op2->clone()) : op2->clone()
            );
            return cast;
        }
    }
    else {
        return std::visit([](auto&& arg) -> std::unique_ptr<ast::literal> {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_arithmetic_v<T>) {
                return std::make_unique<ast::literal>(static_cast<to>(arg));
            }
            else {
                return nullptr;
            }
        }, old_lit->m_value);
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
    const control_flow_node* current_node = &condition_start;
    const control_flow_node* failure_exit = nullptr;
    const control_flow_node* success_exit = nullptr;

    expr_uptr condition = get_expression_as_condition(current_node->m_lines.back().m_instruction.operand1);
    proper_head = current_node->m_index;
    proper_successor = current_node->m_directSuccessor;
    proper_destination = current_node->m_targetSuccessor;

    while (true) {
        const auto& last_line = current_node->m_lines.back();
        const auto  target = current_node->m_targetSuccessor;

        const bool is_and = last_line.m_instruction.opcode == Opcode::BranchIfNot;
        const bool is_or = last_line.m_instruction.opcode == Opcode::BranchIf;

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
                proper_head = current_node->m_index;
                proper_successor = current_node->m_directSuccessor;
                proper_destination = target;
            }
        }
        else if (exit_node->m_index != target) {
            break;
        }
        else {
            parse_basic_block(*current_node);
            condition = std::make_unique<ast::logical_expr>(
                token, std::move(condition),
                get_expression_as_condition(last_line.m_instruction.operand1)
            );
            proper_head = current_node->m_index;
            proper_successor = current_node->m_directSuccessor;
            proper_destination = target;
        }

        current_node = &m_graph[current_node->m_directSuccessor];
    }

    return condition;
}

void decomp_function::insert_return(const reg_idx dest) {
    m_returnType = m_transformableExpressions[dest]->get_type(m_env);
    append_to_current_block(std::make_unique<ast::return_stmt>(std::move(m_transformableExpressions[dest])));
}



}
