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
    for (const auto& statement : m_frame.m_baseBlock.m_statements) {
        out << *statement << "\n";
    }
    return out.str();
} 

[[nodiscard]] std::vector<std::pair<std::string, decompiled_function>> Decompiler::decompile() {
    std::vector<std::pair<std::string, decompiled_function>> funcs;

    for (const auto &func : m_functions) {
        ControlFlowGraph cfg{func};
        cfg.find_loops();
        //cfg.write_image("C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/images/" + func->m_id + ".svg");

        decompiled_function fn{
            std::set<node_id>{},
            expression_frame{func->m_stackFrame.m_symbolTableEntries},
            std::move(cfg)
        };

        emit_node(fn.m_graph[0], fn);

        funcs.emplace_back(func->m_id, std::move(fn));
    }
    return funcs;
}

void Decompiler::emit_node(const control_flow_node& node, decompiled_function& fn) {
    const node_id node_id = node.m_startLine;
    if (fn.m_parsedNodes.contains(node_id)) {
        return;
    }
    fn.m_parsedNodes.insert(node_id);
    parse_basic_block(node, fn.m_frame);
    if (node.m_successors.empty()) {
        return;
    } else if (const auto loop = fn.m_graph.get_loop_with_head(node_id)) {
        return;
        fn.m_frame.insert_loop_head(loop->get(), fn.m_graph[loop->get().m_headNode].m_lines.back().m_instruction.operand1);
        for (const auto& successor : node.m_successors) {
            emit_node(fn.m_graph[successor], fn);
        }
        fn.m_frame.m_blockStack.pop();
    } else {
        for (const auto& successor : node.m_successors) {
            emit_node(fn.m_graph[successor], fn);
        }
    }
}

void Decompiler::parse_basic_block(const control_flow_node &node, expression_frame &expression_frame) {
    for (const auto &line : node.m_lines) {
        const Instruction &istr = line.m_instruction;
        switch(istr.opcode) {
            case Opcode::IAdd: 
            case Opcode::FAdd: expression_frame.apply_binary_op<ast::add_expr>(istr, compiler::token{compiler::token_type::PLUS, "+"}); break;
            case Opcode::ISub:
            case Opcode::FSub: expression_frame.apply_binary_op<ast::sub_expr>(istr, compiler::token{compiler::token_type::MINUS, "-"}); break;
            case Opcode::IMul:
            case Opcode::FMul: expression_frame.apply_binary_op<ast::mul_expr>(istr, compiler::token{compiler::token_type::STAR, "*"}); break;
            case Opcode::IDiv:
            case Opcode::FDiv: expression_frame.apply_binary_op<ast::div_expr>(istr, compiler::token{compiler::token_type::SLASH, "/"}); break;

            case Opcode::IAddImm: expression_frame.apply_binary_op_imm<ast::add_expr>(istr, compiler::token{compiler::token_type::PLUS, "+"}); break;

            case Opcode::IEqual:
            case Opcode::FEqual: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::EQUAL_EQUAL, "=="}); break;
            case Opcode::INotEqual:
            case Opcode::FNotEqual: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::BANG_EQUAL, "!="}); break;
            case Opcode::ILessThan:
            case Opcode::FLessThan: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::LESS, "<"}); break;
            case Opcode::ILessThanEqual:
            case Opcode::FLessThanEqual: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::GREATER_EQUAL, ">="}); break;
            case Opcode::IGreaterThan:
            case Opcode::FGreaterThan: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::GREATER, ">"}); break;
            case Opcode::IGreaterThanEqual:
            case Opcode::FGreaterThanEqual: expression_frame.apply_binary_op<ast::compare_expr>(istr, compiler::token{compiler::token_type::GREATER_EQUAL, ">="}); break;

            case Opcode::OpLogNot: expression_frame.apply_unary_op<ast::logical_not_expr>(istr, compiler::token{compiler::token_type::BANG, "!"}); break;
            case Opcode::OpBitNot: expression_frame.apply_unary_op<ast::bitwise_not_expr>(istr, compiler::token{compiler::token_type::TILDE, "~"}); break;
            case Opcode::INeg:
            case Opcode::FNeg: expression_frame.apply_unary_op<ast::bitwise_not_expr>(istr, compiler::token{compiler::token_type::MINUS, "-"}); break;

            case Opcode::LoadU16Imm: expression_frame.load_literal(istr.destination, static_cast<u16>(istr.operand1 | static_cast<u16>(istr.operand2) << 8)); break;
            case Opcode::LoadStaticInt: expression_frame.load_literal(istr.destination, expression_frame.m_symbolTable[istr.operand1].m_i64); break;
            case Opcode::LoadStaticFloat: expression_frame.load_literal(istr.destination, expression_frame.m_symbolTable[istr.operand1].m_f32); break;
            case Opcode::LoadStaticI32Imm: expression_frame.load_literal(istr.destination, expression_frame.m_symbolTable[istr.operand1].m_i32); break;

            case Opcode::Call:
            case Opcode::CallFf: {
                expression_frame.call(istr);
                break;
            }

            case Opcode::LoadStaticPointer:
            case Opcode::LoadStaticU64Imm:
            case Opcode::LookupPointer: {
                const sid64 sid = expression_frame.m_symbolTable[istr.operand1].m_hash;
                const char* name = m_sidbase.search(sid);
                const ast::sid_literal sid_literal = {sid, name ? name : "test"};
                expr_uptr& lit = expression_frame.load_literal(istr.destination, sid_literal);
                if (expression_frame.m_symbolTable[istr.operand1].m_type != SymbolTableEntryType::FUNCTION) {
                    expression_frame.load_expression_into_var(istr.destination, std::move(lit));
                }
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
            case Opcode::BreakFlag:
            case Opcode::CastInteger:
            case Opcode::CastFloat: {
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