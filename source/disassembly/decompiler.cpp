#define GVDLL

#include "decompiler.h"
#include <fstream>
#include <iostream>
#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>
#include <sstream>
#include <set>
#include <mutex>
#include <chrono>


namespace dconstruct {

void Decompiler::write_control_flow_graph_txt_file(const std::string &path) const noexcept {
    std::ofstream graph_file(path);

    if (!graph_file.is_open()) {
        std::cerr << "couldn't open out graph file " << path << '\n';
    }
    graph_file << "#nodes\n";
    for (const auto& [loc, node] : m_nodes) {
        graph_file << loc << ' ';
        for (const auto& line : node->m_lines) {
            graph_file << line.m_text << ';';
        }
        graph_file << '\n';
    }

    graph_file << "#edges\n";
    for (const auto& [loc, node] : m_nodes) {
        for (const auto &out : node->m_next) {
            graph_file << loc << ' ' << out->m_startLine << '\n';
        }
    }
}

[[nodiscard]] std::string Decompiler::create_node_text(const ControlFlowNode *node) const noexcept {
    std::stringstream ss;

    ss << R"(<TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="10">"
    "<TR><TD ALIGN="LEFT" BALIGN="LEFT"><FONT FACE="Consolas">)";

    for (const auto &line : node->m_lines) {
        ss << line.m_text << "<BR/>"; 
    } 

    ss << "</FONT></TD></TR></TABLE>";


    return ss.str();
}

static std::mutex g_graphviz_mutex;

void Decompiler::write_control_flow_graph_image(const std::string &path) const noexcept {
   GVC_t *gvc = gvContext();
   Agraph_t *g = agopen((char *)"G", Agdirected, nullptr);
   Agnode_t *current_node;
   std::lock_guard<std::mutex> lock(g_graphviz_mutex);
   u32 max_node = 0;
   std::map<ControlFlowNode*, Agnode_t*> node_map{};
   for (const auto &[start_line, node] : m_nodes) {
       std::string name = std::to_string(start_line);
       current_node = agnode(g, name.data(), 1);
       node_map[node.get()] = current_node;
       if (start_line > max_node) {
           max_node = start_line;
       }
       const std::string node_html_label = create_node_text(node.get());

       agsafeset_html(current_node, const_cast<char*>("label"), node_html_label.c_str(), "");

       agsafeset(current_node, const_cast<char*>("fontcolor"), "#8ADCFE", "");
       agsafeset(current_node, const_cast<char*>("shape"), "plaintext", "");
       agsafeset(current_node, const_cast<char*>("color"), "#8ADCFE", "");
   }
   
   for (const auto &[start_line, node] : m_nodes) {
       for (const auto &next : node->m_next) {
           Agedge_t *edge = agedge(g, node_map[node.get()], node_map[next], const_cast<char*>(""), 1);
           if (node->m_endLine + 1 == next->m_startLine) {
               agsafeset(edge, const_cast<char*>("color"), "purple", "");
           } else if (next->m_startLine < node->m_startLine) {
               agsafeset(edge, const_cast<char*>("color"), "red", "");
           } else if (node->m_lines.back().m_instruction.opcode == Opcode::Branch) {
               agsafeset(edge, const_cast<char*>("color"), "blue", "");
           } else {
               agsafeset(edge, const_cast<char*>("color"), "#8ADCFE", "");
           }
       }
   }
   Agraph_t *returng = agsubg(g, const_cast<char*>("return"), 1);
   Agnode_t *return_node = agnode(returng, const_cast<char*>(std::to_string(max_node).c_str()), 1);
   agsafeset(return_node, const_cast<char*>("peripheries"), "1", "");
   agsafeset(returng, const_cast<char*>("rank"), "max", "");
   agsafeset(g, const_cast<char*>("bgcolor"), "#0F0F0F", const_cast<char*>(""));
   agsafeset(g, const_cast<char*>("splines"), "ortho", const_cast<char*>(""));
   gvLayout(gvc, g, "dot");
   gvRenderFilename(gvc, g, "svg", path.c_str());
   gvFreeLayout(gvc, g);
   agclose(g);
   gvFreeContext(gvc);
}

void Decompiler::parse_control_flow_graph() noexcept {
    const std::vector<u32> &labels = m_functionDisassembly->m_stackFrame.m_labels;

    m_nodes[0] = std::make_unique<ControlFlowNode>(0);
    ControlFlowNode *current_node = m_nodes[0].get();
    ControlFlowNode *target_node, *following_node;

    for (u32 i = 0; i < m_functionDisassembly->m_lines.size(); ++i) {
        const FunctionDisassemblyLine &current_line = m_functionDisassembly->m_lines[i];
        current_node->m_lines.push_back(current_line);
        if (current_line.m_instruction.opcode == Opcode::Return) {
            return;
        }
        const FunctionDisassemblyLine &next_line = m_functionDisassembly->m_lines[i + 1];

        b8 next_line_is_target = std::find(labels.begin(), labels.end(), next_line.m_location) != labels.end();

        if (current_line.m_target != -1) {
            if (!m_nodes.contains(current_line.m_target)) {
                m_nodes[current_line.m_target] = std::make_unique<ControlFlowNode>(current_line.m_target);
            }

            target_node = m_nodes[current_line.m_target].get();
            current_node->m_next.push_back(target_node);

            if (!m_nodes.contains(next_line.m_location)) {
                m_nodes[next_line.m_location] = std::make_unique<ControlFlowNode>(next_line.m_location);
            }

            following_node = m_nodes[next_line.m_location].get();

            if (current_line.m_instruction.opcode != Opcode::Branch) {
                current_node->m_next.push_back(following_node);
            }

            current_node->m_endLine = current_line.m_location;
            current_node = following_node;
        }
        else if (next_line_is_target) {
            if (!m_nodes.contains(next_line.m_location)) {
                m_nodes[next_line.m_location] = std::make_unique<ControlFlowNode>(next_line.m_location);
            }

            following_node = m_nodes[next_line.m_location].get();
            current_node->m_next.push_back(following_node);

            current_node->m_endLine = current_line.m_location;
            current_node = following_node;
        } 
    }
    m_controlFlowHead = m_nodes[0].get();
}

void Decompiler::compute_predecessors() noexcept {
    for (const auto &[start_line, node] : m_nodes) {
        for (const auto &successor : node->m_next) {
            m_predecessors[successor->m_startLine].push_back(start_line);
        }
    }
}

[[nodiscard]] std::vector<u32> Decompiler::get_idom_predecessors(const u32 n) const noexcept {
    std::vector<u32> res{};
    for (const auto &p : m_predecessors.at(n)) {
        if (m_immediateDominators.contains(p)) {
            res.push_back(p);
        }
    }
    return res;
}

void Decompiler::find_immediate_dominators() noexcept {
    compute_predecessors();
    m_immediateDominators[0] = 0;

    b8 changed = true;
    while (changed) {
        changed = false;
        for (u32 n = 1; n < m_nodes.size(); ++n) {
            const std::vector<u32> &idom_preds = get_idom_predecessors(n);
            if (idom_preds.empty()) {
                continue;
            }
            u32 new_idom = idom_preds[0];
            for (u32 p = 1; p < idom_preds.size(); ++p) {
                new_idom = intersect(new_idom, p);
            }
            if (m_immediateDominators.at(n) != new_idom) {
                m_immediateDominators[n] = new_idom;
                changed = true;
            }
        }
    }
}


}