#include "decompilation/decomp_function.h"
#include <sstream>
#include <iostream>

namespace dconstruct::dcompiler {


static const auto and_token = compiler::token{ compiler::token_type::AMPERSAND_AMPERSAND, "&&" };
static const auto or_token = compiler::token{ compiler::token_type::PIPE_PIPE, "||" };

decomp_function::decomp_function(const function_disassembly &func, const BinaryFile &current_file) :
    m_disassembly{func}, 
    m_file{current_file}, 
    m_graph{func}, 
    m_parsedNodes(m_graph.m_nodes.size(), false), 
    m_ipdomsEmitted(m_graph.m_nodes.size(), false)
{
#ifdef _DEBUG
    std::cout << "parsing graph " << m_disassembly.m_id << '\n';
    if (m_graph.m_nodes.size() > 1) {
        m_graph.write_image(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\images\)" + m_disassembly.m_id + ".svg");
    }
#endif

    m_blockStack.push(std::ref(m_baseBlock));
    m_transformableExpressions.resize(ARGUMENT_REGISTERS_IDX);
    for (reg_idx i = 0; i < ARGUMENT_REGISTERS_IDX; ++i) {
        m_registersToVars.emplace(i, std::stack<std::unique_ptr<ast::identifier>>());
    }
    for (reg_idx i = ARGUMENT_REGISTERS_IDX; i < MAX_REGISTER; ++i) {
        m_transformableExpressions.emplace_back(std::make_unique<ast::identifier>(compiler::token{ compiler::token_type::IDENTIFIER, "arg_" + std::to_string(i - ARGUMENT_REGISTERS_IDX)}));
        m_registersToVars.emplace(i, std::stack<std::unique_ptr<ast::identifier>>());
    }

    for (reg_idx i = 0; i < func.m_stackFrame.m_registerArgs.size(); ++i) {
        const auto& arg_type = func.m_stackFrame.m_registerArgs.at(i);
        m_arguments.push_back(ast::variable_declaration(arg_type, "arg_" + std::to_string(i)));
    }

    emit_node(m_graph[0], m_graph.m_nodes.back().m_index);
    parse_basic_block(m_graph.m_nodes.back());
}

[[nodiscard]] std::string decomp_function::to_string() const {
    std::ostringstream out;
    const std::string return_type = m_disassembly.m_isScriptFunction ? "void" : ast::type_to_declaration_string(m_returnType);
    out << return_type << ' ' << m_disassembly.m_id << '(';
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
        #ifdef _DEBUG
		std::cout << "parsing instruction " << std::hex << line.m_location << '\n';
        #endif
        const Instruction &istr = line.m_instruction;

        expr_uptr generated_expression = nullptr;

        const auto& symbol_table = m_disassembly.m_stackFrame.m_symbolTable.first;

        switch(istr.opcode) {
            case Opcode::IAdd:
            case Opcode::FAdd: generated_expression = apply_binary_op<ast::add_expr>(istr); break;
            case Opcode::ISub:
            case Opcode::FSub: generated_expression = apply_binary_op<ast::sub_expr>(istr); break;
            case Opcode::IMul:
            case Opcode::FMul: generated_expression = apply_binary_op<ast::mul_expr>(istr); break;
            case Opcode::IDiv:
            case Opcode::FDiv: generated_expression = apply_binary_op<ast::div_expr>(istr); break;
            case Opcode::IMod:
            case Opcode::FMod: generated_expression = apply_binary_op<ast::mod_expr>(istr); break;
            
            case Opcode::IntAsh: generated_expression = make_shift(istr); break;

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
			
            case Opcode::OpBitAnd: generated_expression = apply_binary_op<ast::bitwise_and_expr>(istr); break;
			case Opcode::OpBitOr: generated_expression = apply_binary_op<ast::bitwise_or_expr>(istr); break;
			case Opcode::OpBitXor: generated_expression = apply_binary_op<ast::bitwise_xor_expr>(istr); break;
            case Opcode::OpBitNor: {
                m_transformableExpressions[istr.operand1] = apply_binary_op<ast::bitwise_or_expr>(istr);
                generated_expression = apply_unary_op<ast::bitwise_not_expr>(istr);
                break;
            }

            case Opcode::OpLogNot: generated_expression = apply_unary_op<ast::logical_not_expr>(istr); break;
            case Opcode::OpBitNot: generated_expression = apply_unary_op<ast::bitwise_not_expr>(istr); break;
            case Opcode::INeg:
            case Opcode::FNeg: generated_expression = apply_unary_op<ast::negate_expr>(istr); break;

            case Opcode::LoadU16Imm: generated_expression = std::make_unique<ast::literal>(static_cast<u16>(istr.operand1 | static_cast<u16>(istr.operand2) << 8)); break;

            case Opcode::LoadStaticInt: generated_expression = std::make_unique<ast::cast_expr>(make_type(ast::primitive_kind::U64), make_load_symbol_table(istr)); break;
            case Opcode::LoadStaticFloat: generated_expression = std::make_unique<ast::cast_expr>(make_type(ast::primitive_kind::F32), make_load_symbol_table(istr)); break;
            case Opcode::LoadStaticPointer: generated_expression = std::make_unique<ast::cast_expr>(ast::ptr_type{}, make_load_symbol_table(istr)); break;

			case Opcode::LoadStaticI8Imm: generated_expression = std::make_unique<ast::literal>(symbol_table.get<i8>(istr.operand1 * 8)); break;
			case Opcode::LoadStaticU8Imm: generated_expression = std::make_unique<ast::literal>(symbol_table.get<u8>(istr.operand1 * 8)); break;
			case Opcode::LoadStaticI16Imm: generated_expression = std::make_unique<ast::literal>(symbol_table.get<i16>(istr.operand1 * 8)); break;
			case Opcode::LoadStaticU16Imm: generated_expression = std::make_unique<ast::literal>(symbol_table.get<u16>(istr.operand1 * 8)); break;
            case Opcode::LoadStaticI32Imm: generated_expression = std::make_unique<ast::literal>(symbol_table.get<i32>(istr.operand1 * 8)); break;
			case Opcode::LoadStaticU32Imm: generated_expression = std::make_unique<ast::literal>(symbol_table.get<u32>(istr.operand1 * 8)); break;
            case Opcode::LoadStaticI64Imm: generated_expression = std::make_unique<ast::literal>(symbol_table.get<i64>(istr.operand1 * 8)); break;


            case Opcode::IAbs: generated_expression = make_abs<ast::primitive_kind::U64>(istr.destination); break;
            case Opcode::FAbs: generated_expression = make_abs<ast::primitive_kind::F32>(istr.destination); break;
            

            case Opcode::Call:
            case Opcode::CallFf: {
                auto call = make_call(istr);
                u16 call_usage_count = m_graph.get_register_read_count(node, istr.destination, line.m_location - node.m_startLine + 1);
                if (call_usage_count == 0) {
                    append_to_current_block(std::make_unique<ast::expression_stmt>(std::move(call)));
                }
                /*else if (call_usage_count == 1) {
                    generated_expression = std::move(call);
                }*/
                else {
                    m_transformableExpressions[istr.destination] = std::move(call);
                    load_expression_into_new_var(istr.destination);
                }
                break;
            }
            case Opcode::LoadStaticFloatImm: {
                generated_expression = std::make_unique<ast::literal>(symbol_table.get<f32>(istr.operand1 * 8));
                break;
            }
            case Opcode::LoadStaticPointerImm: {
                if (symbol_table.get<p64>(istr.operand1 * 8) >= (p64)(m_file.m_strings.m_ptr)) {
                    generated_expression = std::make_unique<ast::literal>(symbol_table.get<const char*>(istr.operand1 * 8));
                } else {
                    generated_expression = std::make_unique<ast::literal>("");
                }
                break;
            }
            case Opcode::LoadStaticU64Imm:
            case Opcode::LookupPointer: {
                const sid64 sid = symbol_table.get<sid64>(istr.operand1 * 8);
                const std::string& name = m_file.m_sidCache.at(sid);
                expr_uptr lit = std::make_unique<ast::literal>(sid_literal{ sid, name });
                if (!std::holds_alternative<ast::function_type>(m_disassembly.m_stackFrame.m_symbolTable.second[istr.operand1])) {
                    generated_expression = std::move(lit);
                    generated_expression->set_type(m_disassembly.m_stackFrame.m_symbolTable.second[istr.operand1]);
                }
                else {
                    m_transformableExpressions[istr.destination] = std::move(lit);
                    m_transformableExpressions[istr.destination]->set_type(m_disassembly.m_stackFrame.m_symbolTable.second[istr.operand1]);
                }
                break;
            }

            case Opcode::CastInteger: generated_expression = make_cast<f32, i32>(istr, make_type(ast::primitive_kind::I32)); break;
            case Opcode::CastFloat: generated_expression = make_cast<i32, f32>(istr, make_type(ast::primitive_kind::F32)); break;

            case Opcode::Move: generated_expression = m_transformableExpressions[istr.operand1]->clone(); break;

            case Opcode::LoadFloat: generated_expression = std::make_unique<ast::dereference_expr>(make_cast<u64, u64>(istr, ast::ptr_type{ ast::primitive_kind::F32 })); break;
            case Opcode::LoadI32: generated_expression = std::make_unique<ast::dereference_expr>(make_cast<u64, u64>(istr, ast::ptr_type{ ast::primitive_kind::I32 })); break;
            case Opcode::LoadU64: generated_expression = std::make_unique<ast::dereference_expr>(make_cast<u64, u64>(istr, ast::ptr_type{ ast::primitive_kind::U64 })); break;
            case Opcode::LoadU8: generated_expression = std::make_unique<ast::dereference_expr>(make_cast<u64, u64>(istr, ast::ptr_type{ ast::primitive_kind::U8 })); break;
            case Opcode::LoadU32: generated_expression = std::make_unique<ast::dereference_expr>(make_cast<u64, u64>(istr, ast::ptr_type{ ast::primitive_kind::U32 })); break;
            case Opcode::LoadU16: generated_expression = std::make_unique<ast::dereference_expr>(make_cast<u64, u64>(istr, ast::ptr_type{ ast::primitive_kind::U16 })); break;
            case Opcode::LoadI64: generated_expression = std::make_unique<ast::dereference_expr>(make_cast<u64, u64>(istr, ast::ptr_type{ ast::primitive_kind::I64 })); break;
            case Opcode::LoadPointer: generated_expression = apply_unary_op<ast::dereference_expr>(istr); break;

            case Opcode::StoreU8 : generated_expression = make_store<ast::primitive_kind::U8>(istr); break;
            case Opcode::StoreI8: generated_expression = make_store<ast::primitive_kind::I8>(istr); break;
            case Opcode::StoreU16: generated_expression = make_store<ast::primitive_kind::U16>(istr); break;
            case Opcode::StoreI16: generated_expression = make_store<ast::primitive_kind::I16>(istr); break;
            case Opcode::StoreU32: generated_expression = make_store<ast::primitive_kind::U32>(istr); break;
            case Opcode::StoreI32: generated_expression = make_store<ast::primitive_kind::I32>(istr); break;
            case Opcode::StoreU64: generated_expression = make_store<ast::primitive_kind::U64>(istr); break;
            case Opcode::StoreI64:
            case Opcode::StoreInt: generated_expression = make_store<ast::primitive_kind::I64>(istr); break;
            case Opcode::StoreFloat: generated_expression = make_store<ast::primitive_kind::F32>(istr); break;
            case Opcode::StorePointer: generated_expression = make_store<ast::primitive_kind::U64>(istr); break;


            case Opcode::AssertPointer: m_transformableExpressions[istr.destination]->set_type(ast::ptr_type{}); break;
            
            case Opcode::BreakFlag:
            case Opcode::Branch:
            case Opcode::BranchIf:
            case Opcode::BranchIfNot: {
                continue;
            }

            case Opcode::Return:{
				if (!m_disassembly.m_isScriptFunction) {
                    insert_return(istr.destination); 
                }
                return;
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
        emit_loop(*loop, stop_node);
    }
    else if (last_line.m_instruction.opcode == Opcode::BranchIf || last_line.m_instruction.opcode == Opcode::BranchIfNot) {
        parse_basic_block(node);
        if (node.m_ipdom == node.m_targetNode) {
            emit_if(node, stop_node);
        } else {
            emit_if_else(node, stop_node);
        }
    }
    else if (const auto loop = m_graph.get_loop_with_head(node.m_followingNode)) {
        if (node.m_followingNode != stop_node) {
            parse_basic_block(node);
            emit_loop(*loop, stop_node);
        }
    }
    else {
        parse_basic_block(node);
    }
}

void decomp_function::emit_if(const control_flow_node& node, const node_id stop_node) {
    const node_id idom = node.m_ipdom;
    m_ipdomsEmitted[idom] = true;

    const reg_idx check_register = node.m_lines.back().m_instruction.operand1;

    assert(node.has_target());
    const control_flow_node* target = &m_graph[node.m_targetNode];

    const control_flow_node* current_node = &m_graph[node.m_followingNode];

    expr_uptr final_condition = m_transformableExpressions[check_register]->clone();

    while (current_node != target) {
        parse_basic_block(*current_node);
        const auto& token = current_node->m_lines.back().m_instruction.opcode == Opcode::BranchIf ? or_token : and_token;
        final_condition = std::make_unique<ast::compare_expr>(token, m_transformableExpressions[check_register]->clone(), std::move(final_condition));
        current_node = current_node->m_followingNode ? &m_graph[current_node->m_followingNode] : target;
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
    for (u8 i = 0; i < sizeof(standard_for_loop_pattern); ++i) {
        if (lines[i].m_instruction.opcode != standard_for_loop_pattern[i]) {
            return false;
        }
    }
    return true;
}


void decomp_function::emit_for_loop(const control_flow_loop& loop, const node_id stop_node) {
    const control_flow_node& head_node = m_graph[loop.m_headNode];
    const node_id loop_entry = head_node.m_followingNode;
    const node_id loop_tail = head_node.m_targetNode;
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

        parse_basic_block(current_node);
        auto current_expression = get_expression_as_condition(last_line.m_instruction.operand1);
        if (current_expression == nullptr) {
            throw std::runtime_error("error: current expression was nullptr");
        }
        if (!condition) {
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
    const node_id loop_tail = m_graph[loop.m_headNode].m_targetNode;
    auto loop_block = std::make_unique<ast::block>();
    const node_id head_ipdom = m_graph[loop.m_headNode].m_ipdom;
    const node_id exit_node = loop.m_latchNode + 1;
    const node_id proper_loop_head = m_graph.get_final_loop_condition_node(loop, exit_node).m_index;
    const node_id idom = m_graph[loop_tail].m_ipdom;

    const control_flow_node& loop_entry = m_graph[m_graph[proper_loop_head].m_followingNode];

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


void decomp_function::emit_if_else(const control_flow_node &node, node_id stop_node) {
    const node_id idom = node.m_ipdom;
    const bool idom_already_emitted = m_ipdomsEmitted[idom];
    m_ipdomsEmitted[idom] = true;
    node_id proper_successor, proper_destination, proper_head;
    expr_uptr condition = make_condition(node, proper_head, proper_successor, proper_destination);
    auto then_block = std::make_unique<ast::block>();
    auto else_block = std::make_unique<ast::block>();
    reg_set regs_to_emit = m_graph.get_branch_phi_registers(m_graph[proper_head]);
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
        if (m_registersToVars[reg].empty()) {
            throw std::runtime_error("error: register_to_vars is empty");
        }
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
    m_transformableExpressions[dst] = std::move(id);
    m_transformableExpressions[dst]->set_type(type);
}

void decomp_function::load_expression_into_existing_var(const reg_idx dst, std::unique_ptr<ast::identifier>&& var) {
    if (!m_transformableExpressions[dst]) {
        throw std::runtime_error("error: expressions was empty at " + std::to_string(dst));
    }
    auto& expr = m_transformableExpressions[dst];
    auto rhs_ptr = dynamic_cast<ast::identifier*>(expr.get());
    if (rhs_ptr && rhs_ptr->m_name == var->m_name) {
        return;
    }
    const auto type_temp = expr->get_type(m_env);
    auto assign_expr = std::make_unique<ast::assign_expr>(var->clone(), std::move(expr));

    auto assign_statement = std::make_unique<ast::expression_stmt>(std::move(assign_expr));

    append_to_current_block(std::move(assign_statement));
    m_transformableExpressions[dst] = std::move(var);
    m_transformableExpressions[dst]->set_type(type_temp);
}

template<ast::primitive_kind kind>
[[nodiscard]] std::unique_ptr<ast::call_expr> decomp_function::make_abs(const reg_idx dst) {
    std::vector<expr_uptr> arg;
    arg.push_back(m_transformableExpressions[dst]->clone());
    auto callee = std::make_unique<ast::identifier>("abs");
    auto call = std::make_unique<ast::call_expr>(compiler::token{ compiler::token_type::_EOF, "" }, std::move(callee), std::move(arg));
    call->set_type(make_type(kind));
    return call;
}

[[nodiscard]] std::unique_ptr<ast::call_expr> decomp_function::make_call(const Instruction& istr) {
    expr_uptr callee = m_transformableExpressions[istr.destination]->clone();
    std::vector<expr_uptr> args;
    const auto& callee_type = callee->get_type(m_env);
    if (!std::holds_alternative<ast::function_type>(callee_type)) {
        throw std::runtime_error("error: trying to call non-function");
    }
    ast::function_type func_type = std::get<ast::function_type>(callee_type);

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
    if (!old_lit) {
        const auto& op2 = m_transformableExpressions[istr.operand1];
        const ast::cast_expr* old_cast = dynamic_cast<ast::cast_expr*>(op2.get());
        if (old_cast) {
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
        return std::visit([&type, &old_lit](auto&& arg) -> expr_uptr {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_arithmetic_v<T>) {
                return std::make_unique<ast::literal>(static_cast<to>(arg));
            }
            else {
                return std::make_unique<ast::cast_expr>(type, old_lit->clone());
            }
        }, old_lit->m_value);
    }
}

[[nodiscard]] expr_uptr decomp_function::get_expression_as_condition(const reg_idx src) const {
    if (m_transformableExpressions[src] == nullptr) {
        throw std::runtime_error("error: empty expression at " + std::to_string(src));
    }
    expr_uptr condition = m_transformableExpressions[src]->clone();
    if (std::holds_alternative<ast::ptr_type>(condition->get_type(m_env)) || std::holds_alternative<ast::function_type>(condition->get_type(m_env))) {
        condition = std::make_unique<ast::compare_expr>(compiler::token{ compiler::token_type::BANG_EQUAL, "!=" }, std::move(condition), std::make_unique<ast::literal>(nullptr));
    }
    return condition;
}

// [[nodiscard]] expr_uptr decomp_function::make_condition(const control_flow_node& condition_start) {
//     struct stacked_condition {
//         expr_uptr cond;
//         node_id exit;
//         bool is_or;
//     };

//     const auto and_func = std::make_unique<ast::call_expr>(
//         compiler::token{ compiler::token_type::_EOF, "" },
//         std::make_unique<ast::identifier>("and"),
//         std::vector<expr_uptr>{}
//     );

//     const auto or_func = std::make_unique<ast::call_expr>(
//         compiler::token{ compiler::token_type::_EOF, "" },
//         std::make_unique<ast::identifier>("or"),
//         std::vector<expr_uptr>{}
//     );

//     auto &start = condition_start.m_lines.back().m_instruction.opcode == Opcode::BranchIf ? or_func : and_func;

//     std::vector<stacked_condition> condition_stack;
// 	condition_stack.emplace_back(ast::clone_cast(start), condition_start.m_targetNode, condition_start.m_lines.back().m_instruction.opcode == Opcode::BranchIf);

//     const control_flow_node* current_node = &condition_start;
// 	const control_flow_node* following_node = &m_graph[condition_start.m_followingNode];

//     while (current_node->m_index != condition_start.m_ipdom) {
//         const reg_idx check_register = current_node->m_lines.back().m_instruction.operand1;
        
//         const bool is_or = current_node->m_lines.back().m_instruction.opcode == Opcode::BranchIf;
//         const auto exit = current_node->m_targetNode;
//         auto current_condition = std::unique_ptr<ast::call_expr>(static_cast<ast::call_expr*>(condition_stack.back().cond.release()));
//         auto last_condition_exit = condition_stack.back().exit;
// 		bool last_condition_is_or = condition_stack.back().is_or;
// 		std::cout << "new condition at " << current_node->m_index;
// 		std::cout << " with callee " << (is_or ? "or" : "and") << " and target " << exit << "\n";
//         condition_stack.pop_back();
//         while (true) {
// 			std::cout << "processing next node in chain: " << current_node->m_index << "\n";
//             parse_basic_block(*current_node);
//             current_condition->m_arguments.push_back(get_expression_as_condition(check_register));
           
//             bool fallthrough = current_node->m_lines.back().m_target == following_node->m_lines.back().m_target && following_node->m_lines.back().m_instruction.opcode == current_node->m_lines.back().m_instruction.opcode;
//             bool gets_targeted = last_condition_exit == following_node->m_index;
//             bool done = m_graph[current_node->m_followingNode].m_followingNode == exit;

//             if (done) {
//                 parse_basic_block(*following_node);
//                 std::cout << "done\n";
//                 current_condition->m_arguments.push_back(get_expression_as_condition(check_register));
//                 if (m_graph[following_node->m_followingNode].m_index != condition_start.m_ipdom && m_graph[following_node->m_followingNode].m_lines.back().m_instruction.opcode != Opcode::BranchIf &&
//                     m_graph[following_node->m_followingNode].m_lines.back().m_instruction.opcode != Opcode::BranchIfNot) {
//                     m_transformableExpressions[check_register] = current_condition->clone();
//                     parse_basic_block(m_graph[following_node->m_followingNode]);
//                     current_node = &m_graph[current_node->m_index + 2];
//                     following_node = &m_graph[current_node->m_followingNode];
//                     static_cast<ast::call_expr*>(condition_stack.back().cond.get())->m_arguments.push_back(get_expression_as_condition(check_register));
//                 }
//                 else if (condition_stack.empty()) {
//                     return current_condition;
//                 }
//                 else {
//                     static_cast<ast::call_expr*>(condition_stack.back().cond.get())->m_arguments.push_back(std::move(current_condition));
//                     current_node = following_node;
//                     following_node = &m_graph[following_node->m_followingNode];
//                 }
//                 break;
//             }
//             else if (gets_targeted) {
//                 std::cout << "targeted\n";
//                 auto top_condition = std::unique_ptr<ast::call_expr>(static_cast<ast::call_expr*>(condition_stack.back().cond.release()));
// 				condition_stack.pop_back();
//                 top_condition->m_arguments.push_back(std::move(current_condition));
//                 if (condition_stack.empty()) {
//                     condition_stack.emplace_back(ast::clone_cast(is_or ? or_func : and_func), exit, is_or);
//                 }
//                 static_cast<ast::call_expr&>(*condition_stack.back().cond).m_arguments.push_back(std::move(top_condition));
//                 break;
//             }
//             else if (!fallthrough) {
// 				const auto previous_exit = !condition_stack.empty() ? condition_stack.back().exit : 0;
//                 auto &cond = following_node->m_lines.back().m_instruction.opcode == Opcode::BranchIf ? or_func : and_func;
//                 const bool condi = is_or ? last_condition_is_or : !last_condition_is_or;
//                 if (current_node->m_targetNode == previous_exit && condi) {
// 					std::cout << "inserting as sibling\n";
//                     static_cast<ast::call_expr*>(condition_stack.back().cond.get())->m_arguments.push_back(std::move(current_condition));
//                 }
//                 else {
// 					std::cout << "new condition on stack\n";
//                     condition_stack.emplace_back(std::move(current_condition), last_condition_exit, last_condition_is_or);
//                 }
//                 condition_stack.emplace_back(ast::clone_cast(cond), current_node->m_targetNode, is_or);
//                 break;
//             }
//             current_node = &m_graph[current_node->m_followingNode];
//             following_node = &m_graph[following_node->m_followingNode];
//         }
//         current_node = &m_graph[current_node->m_followingNode];
//         following_node = &m_graph[following_node->m_followingNode];
//         std::cout << "stack: " << condition_stack.size() << "\n";
//     }

// 	return std::move(condition_stack.back().cond);
// }


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
    proper_successor = current_node->m_followingNode;
    proper_destination = current_node->m_targetNode;

    while (true) {
        const auto& last_line = current_node->m_lines.back();
        const auto  target = current_node->m_targetNode;

        const bool is_and = last_line.m_instruction.opcode == Opcode::BranchIfNot;
        const bool is_or = last_line.m_instruction.opcode == Opcode::BranchIf;

        if (!is_and && !is_or)
            break;

        auto& exit_node = is_and ? failure_exit : success_exit;
        auto& alt_exit = is_and ? success_exit : failure_exit;
        const auto& token = is_and ? and_token : or_token;

        if (!exit_node) {
            if (!alt_exit) {
                exit_node = &m_graph[target];
            }
            else {
                parse_basic_block(*current_node);
                condition = std::make_unique<ast::logical_expr>(
                    token, std::move(condition),
                    get_expression_as_condition(last_line.m_instruction.operand1)
                );
                proper_head = current_node->m_index;
                proper_successor = current_node->m_followingNode;
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
            proper_successor = current_node->m_followingNode;
            proper_destination = target;
        }

        current_node = &m_graph[current_node->m_followingNode];
    }

    return condition;
}

void decomp_function::insert_return(const reg_idx dest) {
    m_returnType = m_transformableExpressions[dest]->get_type(m_env);
    append_to_current_block(std::make_unique<ast::return_stmt>(std::move(m_transformableExpressions[dest])));
}

template<ast::primitive_kind kind>
[[nodiscard]] std::unique_ptr<ast::assign_expr> decomp_function::make_store(const Instruction& istr) {
    auto lhs = std::make_unique<ast::dereference_expr>(make_cast<u64, u64>(istr, ast::ptr_type{ kind }));
    auto rhs = m_transformableExpressions[istr.operand2]->clone();
    auto res = std::make_unique<ast::assign_expr>(std::move(lhs), std::move(rhs));
    append_to_current_block(res->clone());
    return res;
}



}
