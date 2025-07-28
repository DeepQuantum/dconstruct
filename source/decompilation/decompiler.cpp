#include "decompiler.h"
#include "tokens.h"
#include "expression_frame.h"
#include <fstream>
#include <iostream>


namespace dconstruct::dcompiler {

std::vector<decompiled_function> Decompiler::decompile() {
    std::vector<decompiled_function> funcs;
    funcs.reserve(m_functions.size());
    for (const auto &func : m_functions) {
        ControlFlowGraph cfg{func};
        cfg.find_loops();
        //cfg.write_image("C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/build/images/" + func->m_id + ".svg");

        expression_frame frame(func->m_stackFrame.m_symbolTableEntries);
        for (const auto& [_, node] : cfg.get_nodes()) {
            parse_basic_block(node, frame);
        }
        funcs.emplace_back(std::move(frame));
    }
    return funcs;
}

void Decompiler::parse_basic_block(const control_flow_node &node, expression_frame &expression_frame) {
    for (const auto &line : node.m_lines) {
        const Instruction &istr = line.m_instruction;
        switch(istr.opcode) {
            case Opcode::IAdd: 
            case Opcode::FAdd: expression_frame.apply_binary_op<ast::add_expr>(istr); break;
            case Opcode::ISub:
            case Opcode::FSub: expression_frame.apply_binary_op<ast::sub_expr>(istr); break;
            case Opcode::IMul:
            case Opcode::FMul: expression_frame.apply_binary_op<ast::mul_expr>(istr); break;
            case Opcode::IDiv:
            case Opcode::FDiv: expression_frame.apply_binary_op<ast::div_expr>(istr); break;

            case Opcode::LoadU16Imm: expression_frame.load_literal(istr.destination, E_INT, u64(istr.operand1 | (istr.operand2 << 8))); break;
            case Opcode::LoadStaticInt: expression_frame.load_literal(istr.destination, E_INT, expression_frame.m_symbolTable[istr.operand1].m_i64);
            case Opcode::LoadStaticFloat: expression_frame.load_literal(istr.destination, E_FLOAT, expression_frame.m_symbolTable[istr.operand1].m_f32);
            case Opcode::LoadStaticPointer: expression_frame.load_literal(istr.destination, E_HASH, expression_frame.m_symbolTable[istr.operand1].m_hash);


            case Opcode::Move: {
                expression_frame.move(istr.destination, istr.operand1);
            }
            default: {
                return;
            }
        }
    }
}
}