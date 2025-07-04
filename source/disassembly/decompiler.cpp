#include "decompiler.h"
#include <fstream>
#include <iostream>

namespace dconstruct {

// void Decompiler::insert_new_node(const u32 target, const ControlFlowNode &node) noexcept {
//     if (!m_nodes.contains(node.m_startLine)) {
//         m_nodes[node.m_startLine] = node;
//     }
//     m_nodes[node.m_startLine].m_incoming
// }

void Decompiler::output_cfg_file(const std::string &path) const noexcept {
    std::ofstream graph_file(path);

    if (!graph_file.is_open()) {
        std::cerr << "couldn't open out graph file " << path << '\n';
    }
    graph_file << "#nodes\n";
    for (const auto& [loc, node] : m_nodes) {
        graph_file << loc << ' ';
        // for (const auto& line : node->m_lines) {
        //     graph_file << line.m_text << ';';
        // }
        graph_file << '\n';
    }

    graph_file << "#edges\n";
    for (const auto& [loc, node] : m_nodes) {
        for (const auto &in : node->m_incoming) {
            graph_file << in->m_startLine << ' ' << loc << '\n';
        }
        for (const auto &out : node->m_outgoing) {
            graph_file << loc << ' ' << out->m_startLine << '\n';
        }
    }
}

void Decompiler::parse_control_flow_graph() noexcept {
    const std::vector<u32> &labels = m_functionDisassembly->m_stackFrame.m_labels;
    const std::vector<u32> &vlabels = m_functionDisassembly->m_stackFrame.m_virtualLabels;

    m_nodes[0] = std::make_unique<ControlFlowNode>(0);
    ControlFlowNode *current_node = m_nodes[0].get();
    ControlFlowNode *target_node, *following_node;

    for (const auto &line : m_functionDisassembly->m_lines) {
        current_node->m_lines.push_back(line);

        b8 gets_pointed_at = std::find(labels.begin(), labels.end(), line.m_location) != labels.end() || 
            std::find(vlabels.begin(), vlabels.end(), line.m_location) != vlabels.end();

        if (line.m_target != -1) {
            if (!m_nodes.contains(line.m_target)) {
                m_nodes[line.m_target] = std::make_unique<ControlFlowNode>(line.m_target);
            }

            target_node = m_nodes[line.m_target].get();
            target_node->m_incoming.push_back(current_node);
            current_node->m_outgoing.push_back(target_node);

            if (!m_nodes.contains(line.m_location + 1)) {
                m_nodes[line.m_location + 1] = std::make_unique<ControlFlowNode>(line.m_location + 1);
            }
            following_node = m_nodes[line.m_location + 1].get();
            if (line.m_instruction.opcode != Opcode::Branch) {
                following_node->m_incoming.push_back(current_node);
                current_node->m_outgoing.push_back(following_node);
            }

            current_node->m_endLine = line.m_location;
            current_node = following_node;
        }
        if (current_node->m_lines.size() > 1 && gets_pointed_at) {
            if (!m_nodes.contains(line.m_location + 1)) {
                m_nodes[line.m_location + 1] = std::make_unique<ControlFlowNode>(line.m_location + 1);
            }
            following_node = m_nodes[line.m_location + 1].get();
            current_node->m_endLine = line.m_location;

            current_node = following_node;
        } 
    }
    m_controlFlowHead = m_nodes[0].get();
}


}