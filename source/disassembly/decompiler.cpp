#include "decompiler.h"

namespace dconstruct {

void Decompiler::insert_new_node(const u32 target, const ControlFlowNode &node) noexcept {
    if (!m_nodes.contains(node.m_startLine)) {
        m_nodes[node.m_startLine] = node;
    }
    m_nodes[node.m_startLine].m_incoming
}

void Decompiler::parse_control_flow_graph() noexcept {
    ControlFlowNode current{};
    current.m_startLine = 0;
    const std::vector<u32> &labels = m_functionDisassembly->m_stackFrame.m_labels;


    for (const auto &line : m_functionDisassembly->m_lines) {
        current.m_lines.push_back(line);
        if (line.m_target != -1) {
            current.m_outgoing.push_back(line.m_target);
            u32 old_start = current.m_startLine;
            m_nodes[current.m_startLine] = current;
            b8 is_conditional_branch = line.m_instruction.opcode != Opcode::Branch;
            if (is_conditional_branch) {
                current.m_outgoing.push_back(line.m_location + 1);
            }
            current = ControlFlowNode{};
            current.m_startLine = line.m_location + 1;
            if (is_conditional_branch) {
                current.m_incoming.push_back(old_start);
            }
        } 
        if (current.m_lines.size() > 1 && std::find(labels.begin(), labels.end(), line.m_location) != labels.end()) {
            // this block is done, we aren't pointing anywhere but this line gets pointed at.
            // so current block has next block as outgoing, next block has current block as incoming
            // next block has the label as incoming
        } 

    }
}


}