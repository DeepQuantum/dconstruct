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
    out << ") {\n";
    for (const auto& statement : m_frame.m_baseBlock.m_statements) {
        out << "    " << *statement << '\n';
    }
    out << '}';
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

        emit_node(fn.m_graph[0], fn);

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

        parse_basic_block(fn.m_graph[node], fn.m_frame);

        funcs.emplace(func->m_id, std::move(fn));
    }
    return funcs;
}

void Decompiler::emit_node(const control_flow_node& node, decompiled_function& fn) {
    const node_id current_node_id = node.m_startLine;
    if (fn.m_parsedNodes.contains(current_node_id)) {
        return;
    }
    fn.m_parsedNodes.insert(current_node_id);
    parse_basic_block(node, fn.m_frame);
    if (node.m_successors.empty()) {
        return;
    } else if (const auto loop = fn.m_graph.get_loop_with_head(current_node_id)) {
        fn.m_frame.insert_loop_head(loop->get(), fn.m_graph[loop->get().m_headNode].m_lines.back().m_instruction.operand1);
        for (const auto& successor : node.m_successors) {
            emit_node(fn.m_graph[successor], fn);
        }
        fn.m_frame.m_blockStack.pop();
    } else {
        const auto& last_line = node.get_last_line();
        node_id positive_branch_head = 0, negative_branch_head = 0;
        if (last_line.m_instruction.opcode == Opcode::BranchIfNot) {
            positive_branch_head = last_line.m_instruction.operand1;
            negative_branch_head = node.get_direct_successor();
        }
    }
}

void Decompiler::parse_basic_block(const control_flow_node &node, expression_frame &expression_frame) {
    const compiler::token plus = compiler::token{ compiler::token_type::PLUS, "+" };
    const compiler::token minus = compiler::token{ compiler::token_type::MINUS, "-" };
    const compiler::token multiply = compiler::token{ compiler::token_type::STAR, "*" };
    const compiler::token divide = compiler::token{ compiler::token_type::SLASH, "/" };

    for (const auto &line : node.m_lines) {
        const Instruction &istr = line.m_instruction;

        switch(istr.opcode) {
            case Opcode::IAdd: 
            case Opcode::FAdd: expression_frame.apply_binary_op<ast::add_expr>(istr, plus); break;
            case Opcode::ISub:
            case Opcode::FSub: expression_frame.apply_binary_op<ast::sub_expr>(istr, minus); break;
            case Opcode::IMul:
            case Opcode::FMul: expression_frame.apply_binary_op<ast::mul_expr>(istr, multiply); break;
            case Opcode::IDiv:
            case Opcode::FDiv: expression_frame.apply_binary_op<ast::div_expr>(istr, divide); break;

            case Opcode::IAddImm: expression_frame.apply_binary_op_imm<ast::add_expr>(istr, plus); break;
            case Opcode::IMulImm: expression_frame.apply_binary_op_imm<ast::mul_expr>(istr, multiply); break;
            case Opcode::ISubImm: expression_frame.apply_binary_op_imm<ast::sub_expr>(istr, minus); break;
            case Opcode::IDivImm: expression_frame.apply_binary_op_imm<ast::div_expr>(istr, divide); break;

            case Opcode::IEqual:
            case Opcode::FEqual: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::EQUAL_EQUAL, "=="}); break;
            case Opcode::INotEqual:
            case Opcode::FNotEqual: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::BANG_EQUAL, "!="}); break;
            case Opcode::ILessThan:
            case Opcode::FLessThan: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::LESS, "<"}); break;
            case Opcode::ILessThanEqual:
            case Opcode::FLessThanEqual: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::LESS_EQUAL, "<="}); break;
            case Opcode::IGreaterThan:
            case Opcode::FGreaterThan: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::GREATER, ">"}); break;
            case Opcode::IGreaterThanEqual:
            case Opcode::FGreaterThanEqual: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::GREATER_EQUAL, ">="}); break;

            case Opcode::OpLogNot: expression_frame.apply_unary_op<ast::logical_not_expr>(istr, compiler::token{compiler::token_type::BANG, "!"}); break;
            case Opcode::OpBitNot: expression_frame.apply_unary_op<ast::bitwise_not_expr>(istr, compiler::token{compiler::token_type::TILDE, "~"}); break;
            case Opcode::INeg:
            case Opcode::FNeg: expression_frame.apply_unary_op<ast::negate_expr>(istr, compiler::token{compiler::token_type::MINUS, "-"}); break;

            case Opcode::LoadU16Imm: expression_frame.load_literal(istr.destination, static_cast<u16>(istr.operand1 | static_cast<u16>(istr.operand2) << 8)); break;
            case Opcode::LoadStaticInt: expression_frame.load_literal(istr.destination, expression_frame.m_symbolTable.value().get()[istr.operand1].m_i64); break;
            case Opcode::LoadStaticFloat: expression_frame.load_literal(istr.destination, expression_frame.m_symbolTable.value().get()[istr.operand1].m_f32); break;
            case Opcode::LoadStaticI32Imm: expression_frame.load_literal(istr.destination, expression_frame.m_symbolTable.value().get()[istr.operand1].m_i32); break;

            case Opcode::Call:
            case Opcode::CallFf: {
                expression_frame.call(istr);
                break;
            }

            case Opcode::LoadStaticPointer:
            case Opcode::LoadStaticU64Imm:
            case Opcode::LookupPointer: {
                const sid64 sid = expression_frame.m_symbolTable.value().get()[istr.operand1].m_hash;
                const std::string& name = m_currentFile.m_sidCache.at(sid);
                const sid_literal sid_literal = {sid, name};
                expr_uptr& lit = expression_frame.load_literal(istr.destination, sid_literal);
                if (expression_frame.m_symbolTable.value().get()[istr.operand1].m_type != SymbolTableEntryType::FUNCTION) {
                    expression_frame.load_expression_into_var(istr.destination, std::move(lit));
                }
                break;
            }

            //case Opcode::Load

            case Opcode::CastInteger: {
                expression_frame.cast_to_int(istr);
                break;
            }
            case Opcode::CastFloat: {
                expression_frame.cast_to_float(istr);
                break;
            }

            case Opcode::Move: {
                expression_frame.move(istr.destination, istr.operand1);
                break;
            }

            // case Opcode::CastInteger: {
            //     expression_frame.cast(RegisterValueType::I64, istr);
            // }

            case Opcode::AssertPointer:
            case Opcode::BreakFlag: {
                continue;
            }

            case Opcode::Return: expression_frame.insert_return(istr.destination); break;
            default: {
                continue;
            }
        }
    }
}
}