#define GVDLL

#include "decompiler.h"
#include "expression_frame.h"
#include <fstream>
#include <iostream>
#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>
#include <sstream>
#include <set>
#include <mutex>
#include <chrono>


namespace dconstruct::dcompiler {

std::vector<decompiled_function> Decompiler::decompile() noexcept {
    for (const auto &func : m_functions) {
        ControlFlowGraph cfg{func};
        cfg.find_loops();
        cfg.write_image("C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/build/images/" + func->m_id + ".svg");

        expression_frame frame;
        for (const auto& [_, node] : cfg.get_nodes()) {
            parse_basic_block(node, frame);
        }
    }
    return{};
}

void Decompiler::parse_basic_block(const control_flow_node &node, expression_frame &expression_frame) noexcept {
    for (const auto &line : node.m_lines) {
        const Instruction &istr = line.m_instruction;
        switch(istr.opcode) {
            case Opcode::Move: expression_frame.move(istr.destination, istr.operand1); break;
            case Opcode::IAdd: expression_frame.apply_binary_op<add_expr>(istr); break;
            case Opcode::ISub: expression_frame.apply_binary_op<sub_expr>(istr); break;
            case Opcode::IMul: expression_frame.apply_binary_op<mul_expr>(istr); break;
            case Opcode::IDiv: expression_frame.apply_binary_op<div_expr>(istr); break;
            
            default: {
                return;
            }
        }
    }
}
}