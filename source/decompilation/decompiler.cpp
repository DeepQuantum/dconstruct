#include "decompilation/decompiler.h"
#include "compilation/tokens.h"
#include "decompilation/expression_frame.h"
#include "ast/type.h"
#include <fstream>
#include <iostream>
#include <sstream>


namespace dconstruct::dcompiler {

[[nodiscard]] std::string decompiled_function::to_string() const {
    std::ostringstream out;
    out << "function " << m_id << '(';
    for (u32 i = 0; i < m_frame.m_arguments.size(); ++i) {
        out << m_frame.m_arguments[i].m_typeName << ' ' << m_frame.m_arguments[i].m_identifier;
        if (i != m_frame.m_arguments.size() - 1) {
            out << ", ";
        }
    }
    out << ") ";
    out << m_frame.m_baseBlock;
    return out.str();
}


[[nodiscard]] std::unordered_map<std::string, decompiled_function> Decompiler::decompile() {
    std::unordered_map<std::string, decompiled_function> funcs;

    for (const auto &func : m_functions) {
        ControlFlowGraph cfg{func};
        cfg.find_loops();
        //cfg.write_image("C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/images/" + func->m_id + ".svg");

        decompiled_function fn{
            func->m_id,
            std::set<node_id>{},
            expression_frame{ func->m_stackFrame.m_symbolTableEntries },
            std::move(cfg)
        };

        for (u32 i = 0; i < func->m_stackFrame.m_registerArgs.size(); ++i) {
            const ast::full_type arg_type = ast::register_type_to_ast_type(func->m_stackFrame.m_registerArgs.at(i));
            const std::string type_name = ast::type_to_declaration_string(arg_type);
            fn.m_frame.m_arguments.push_back(ast::variable_declaration(type_name, "arg_" + std::to_string(i)));
        }
        //std::set<u32> last_set = fn.m_graph.get_variant_registers(0);

        emit_node(fn.m_graph[0], fn, fn.m_graph.get_return_node().m_startLine);
        parse_basic_block(fn.m_graph.get_return_node(), fn.m_frame, fn.m_graph);


        funcs.emplace(func->m_id, std::move(fn));
    }
    return funcs;
}

[[nodiscard]] std::unordered_map<std::string, decompiled_function> Decompiler::decompile_node(const node_id node) {
    std::unordered_map<std::string, decompiled_function> funcs;

    for (const auto &func : m_functions) {
        ControlFlowGraph cfg{func};
        ControlFlowGraph single_node_cfg{func, cfg.get_nodes()};
        cfg.find_loops();
        //cfg.write_image("C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/images/" + func->m_id + ".svg");

        decompiled_function fn{
            func->m_id,
            std::set<node_id>{},
            expression_frame{ func->m_stackFrame.m_symbolTableEntries },
            std::move(single_node_cfg)
        };

        for (u32 i = 0; i < func->m_stackFrame.m_registerArgs.size(); ++i) {
            const ast::full_type arg_type = ast::register_type_to_ast_type(func->m_stackFrame.m_registerArgs.at(i));
            const std::string type_name = ast::type_to_declaration_string(arg_type);
            fn.m_frame.m_arguments.push_back(ast::variable_declaration(type_name, "arg_" + std::to_string(i)));
        }
        //std::set<u32> last_set = fn.m_graph.get_variant_registers(0);
        parse_basic_block(fn.m_graph[node], fn.m_frame, fn.m_graph);

        funcs.emplace(func->m_id, std::move(fn));
    }
    return funcs;
}

void Decompiler::emit_node(const control_flow_node& node, decompiled_function& fn, node_id stop_node) {
    const node_id current_node_id = node.m_startLine;
    if (fn.m_parsedNodes.contains(current_node_id)) {
        return;
    }
    fn.m_parsedNodes.insert(current_node_id);

    if (current_node_id == stop_node) {
        return;
    }

    parse_basic_block(node, fn.m_frame, fn.m_graph);
    const auto& last_line = node.get_last_line();

    if (const auto loop = fn.m_graph.get_loop_with_head(last_line.m_location + 1)) {
        const control_flow_node& head_node = fn.m_graph[last_line.m_location + 1];
        parse_basic_block(head_node, fn.m_frame, fn.m_graph);
        fn.m_frame.insert_loop_head(loop->get(), head_node.m_lines.back().m_instruction.operand1);
        emit_node(fn.m_graph[head_node.get_last_line().m_location + 1], fn, head_node.m_startLine);
        fn.m_frame.m_blockStack.pop();
    } else if (last_line.m_instruction.opcode == Opcode::BranchIf || last_line.m_instruction.opcode == Opcode::BranchIfNot) {
        const node_id idom = fn.m_graph.get_immediate_postdominators().at(current_node_id);
        expr_uptr condition = fn.m_frame.emit_condition(fn.m_graph[current_node_id]);
        auto then_block = std::make_unique<ast::block>();
        auto else_block = std::make_unique<ast::block>();

        fn.m_frame.m_blockStack.push(*then_block);
        emit_node(fn.m_graph[node.get_direct_successor()], fn, idom);
        fn.m_frame.m_blockStack.pop();

        fn.m_frame.m_blockStack.push(*else_block);
        emit_node(fn.m_graph[last_line.m_instruction.destination], fn, idom);
        fn.m_frame.m_blockStack.pop();
        stmnt_uptr full_if = std::make_unique<ast::if_stmt>(std::move(condition), std::move(then_block), else_block->m_statements.empty() ? nullptr : std::move(else_block));

        fn.m_frame.append_to_current_block(std::move(full_if));

        emit_node(fn.m_graph[idom], fn, stop_node);
    }
}

void Decompiler::parse_basic_block(const control_flow_node &node, expression_frame &expression_frame, const ControlFlowGraph& graph) {
    static const compiler::token plus = compiler::token{ compiler::token_type::PLUS, "+" };
    static const compiler::token minus = compiler::token{ compiler::token_type::MINUS, "-" };
    static const compiler::token multiply = compiler::token{ compiler::token_type::STAR, "*" };
    static const compiler::token divide = compiler::token{ compiler::token_type::SLASH, "/" };

    std::set<u32> registers_to_emit = graph.get_variant_registers(node.m_startLine);

    for (const auto &line : node.m_lines) {
        const Instruction &istr = line.m_instruction;

        expr_uptr generated_expression = nullptr;

        switch(istr.opcode) {
            case Opcode::IAdd: 
            case Opcode::FAdd: generated_expression = expression_frame.apply_binary_op<ast::add_expr>(istr, plus); break;
            case Opcode::ISub:
            case Opcode::FSub: generated_expression = expression_frame.apply_binary_op<ast::sub_expr>(istr, minus); break;
            case Opcode::IMul:
            case Opcode::FMul: generated_expression = expression_frame.apply_binary_op<ast::mul_expr>(istr, multiply); break;
            case Opcode::IDiv:
            case Opcode::FDiv: generated_expression = expression_frame.apply_binary_op<ast::div_expr>(istr, divide); break;

            case Opcode::IAddImm: generated_expression = expression_frame.apply_binary_op_imm<ast::add_expr>(istr, plus); break;
            case Opcode::IMulImm: generated_expression = expression_frame.apply_binary_op_imm<ast::mul_expr>(istr, multiply); break;
            case Opcode::ISubImm: generated_expression = expression_frame.apply_binary_op_imm<ast::sub_expr>(istr, minus); break;
            case Opcode::IDivImm: generated_expression = expression_frame.apply_binary_op_imm<ast::div_expr>(istr, divide); break;

            case Opcode::IEqual:
            case Opcode::FEqual: generated_expression = expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::EQUAL_EQUAL, "=="}); break;
            case Opcode::INotEqual:
            case Opcode::FNotEqual: generated_expression = expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::BANG_EQUAL, "!="}); break;
            case Opcode::ILessThan:
            case Opcode::FLessThan: generated_expression = expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::LESS, "<"}); break;
            case Opcode::ILessThanEqual:
            case Opcode::FLessThanEqual: generated_expression = expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::LESS_EQUAL, "<="}); break;
            case Opcode::IGreaterThan:
            case Opcode::FGreaterThan: generated_expression = expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::GREATER, ">"}); break;
            case Opcode::IGreaterThanEqual:
            case Opcode::FGreaterThanEqual: generated_expression = expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::GREATER_EQUAL, ">="}); break;

            case Opcode::OpLogNot: generated_expression = expression_frame.apply_unary_op<ast::logical_not_expr>(istr, compiler::token{compiler::token_type::BANG, "!"}); break;
            case Opcode::OpBitNot: generated_expression = expression_frame.apply_unary_op<ast::bitwise_not_expr>(istr, compiler::token{compiler::token_type::TILDE, "~"}); break;
            case Opcode::INeg:
            case Opcode::FNeg: generated_expression = expression_frame.apply_unary_op<ast::negate_expr>(istr, compiler::token{compiler::token_type::MINUS, "-"}); break;

            case Opcode::LoadU16Imm: generated_expression = std::make_unique<ast::literal>(static_cast<u16>(istr.operand1 | static_cast<u16>(istr.operand2) << 8)); break;
            case Opcode::LoadStaticInt: generated_expression = std::make_unique<ast::literal>(expression_frame.m_symbolTable.value().get()[istr.operand1].m_i64); break;
            case Opcode::LoadStaticFloat: generated_expression = std::make_unique<ast::literal>(expression_frame.m_symbolTable.value().get()[istr.operand1].m_f32); break;
            case Opcode::LoadStaticI32Imm: generated_expression = std::make_unique<ast::literal>(expression_frame.m_symbolTable.value().get()[istr.operand1].m_i32); break;

            case Opcode::Call:
            case Opcode::CallFf: {
                expr_uptr call = expression_frame.call(istr);
                if (graph.register_gets_read_before_overwrite(node.m_startLine, istr.destination, line.m_location - node.m_startLine)) {
                    generated_expression = std::move(call);
                } else {
                    expression_frame.append_to_current_block(std::make_unique<ast::expression_stmt>(std::move(call)));
                    generated_expression = nullptr;
                }
                break;
            }
            

            case Opcode::LoadStaticPointerImm: {
                std::string string = reinterpret_cast<const char*>(expression_frame.m_symbolTable.value().get()[istr.operand1].m_pointer);
                generated_expression = std::make_unique<ast::literal>(std::move(string));
                break;
            }
            case Opcode::LoadStaticU64Imm:
            case Opcode::LookupPointer: {
                const sid64 sid = expression_frame.m_symbolTable.value().get()[istr.operand1].m_hash;
                const std::string& name = m_currentFile.m_sidCache.at(sid);
                sid_literal sid_literal = {sid, name};
                expr_uptr lit = std::make_unique<ast::literal>(std::move(sid_literal));
                if (expression_frame.m_symbolTable.value().get()[istr.operand1].m_type != SymbolTableEntryType::FUNCTION) {
                    generated_expression = std::move(lit);
                }
                else {
                    expression_frame.m_transformableExpressions[istr.destination] = std::move(lit);
                }
                break;
            }

            case Opcode::CastInteger: {
                generated_expression = expression_frame.cast_to_int(istr);
                break;
            }
            case Opcode::CastFloat: {
                generated_expression = expression_frame.cast_to_float(istr);
                break;
            }

            case Opcode::Move: {
                generated_expression = expression_frame.m_transformableExpressions[istr.operand1]->clone();
                break;
            }

            case Opcode::LoadFloat:
            case Opcode::LoadI32: {
                generated_expression = expression_frame.load_with_dereference(istr);
                break;
            }

            case Opcode::AssertPointer:
            case Opcode::BreakFlag:
            case Opcode::Branch:
            case Opcode::BranchIf:
            case Opcode::BranchIfNot: {
                continue;
            }

            case Opcode::Return: expression_frame.insert_return(istr.destination); break;

            default: {
                throw std::runtime_error("");
            }
        }
         if (generated_expression != nullptr) {
            if (registers_to_emit.contains(istr.destination)) {
                expression_frame.load_expression_into_var(istr.destination, std::move(generated_expression));
            }
            else {
                expression_frame.m_transformableExpressions[istr.destination] = std::move(generated_expression);
            }
         }
    }
}
}