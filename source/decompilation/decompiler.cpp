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
    std::vector<decompiled_function> funcs;
    funcs.reserve(m_functions.size());
    for (const auto &func : m_functions) {
        ControlFlowGraph cfg{func};
        cfg.find_loops();
        //cfg.write_image("C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/build/images/" + func->m_id + ".svg");

        expression_frame frame;
        for (const auto& [_, node] : cfg.get_nodes()) {
            parse_basic_block(node, frame);
        }
        funcs.emplace_back(std::move(frame));
    }
    return funcs;
}

void Decompiler::parse_basic_block(const control_flow_node &node, expression_frame &expression_frame) noexcept {
    for (const auto &line : node.m_lines) {
        const Instruction &istr = line.m_instruction;
        switch(istr.opcode) {
            case Opcode::Move: expression_frame.apply_binary_op<ast::assign_expr>(istr); break;
            case Opcode::IAdd: expression_frame.apply_binary_op<ast::add_expr>(istr); break;
            case Opcode::ISub: expression_frame.apply_binary_op<ast::sub_expr>(istr); break;
            case Opcode::IMul: expression_frame.apply_binary_op<ast::mul_expr>(istr); break;
            case Opcode::IDiv: expression_frame.apply_binary_op<ast::div_expr>(istr); break;
            case Opcode::LoadU16Imm: expression_frame.load_immediate(istr.destination, istr.operand1 | (istr.operand2 << 8)); break;
            default: {
                std::cerr << "Not implemented\n";
            }
        }
    }
}
}