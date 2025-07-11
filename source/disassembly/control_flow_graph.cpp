#define GVDLL

#include "control_flow_graph.h"

#include <fstream>
#include <iostream>
#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>
#include <sstream>
#include <mutex>

namespace dconstruct {

    [[nodiscard]] std::string ControlFlowNode::get_label_html() const noexcept {
        std::stringstream ss;

        ss << R"(<TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="10">"
        "<TR><TD ALIGN="LEFT" BALIGN="LEFT"><FONT FACE="Consolas">)";

        for (const auto& line : m_lines) {
            ss << line.m_text << "<BR/>";
        }

        ss << "</FONT></TD></TR></TABLE>";


        return ss.str();
    } 

    ControlFlowGraph::ControlFlowGraph(const FunctionDisassembly *func) noexcept {
        m_func = func;
        const std::vector<u32> &labels = func->m_stackFrame.m_labels;
        m_nodes[0] = ControlFlowNode(0);
        ControlFlowNode *current_node = &m_nodes[0];
        ControlFlowNode *target_node, *following_node;

        for (u32 i = 0; i < func->m_lines.size(); ++i) {
            const FunctionDisassemblyLine &current_line = func->m_lines[i];
            current_node->m_lines.push_back(current_line);
            if (current_line.m_instruction.opcode == Opcode::Return) {
                current_node->m_endLine = current_line.m_location;
                return;
            }
            const FunctionDisassemblyLine &next_line = func->m_lines[i + 1];

            b8 next_line_is_target = std::find(labels.begin(), labels.end(), next_line.m_location) != labels.end();

            if (current_line.m_target != -1) {
                target_node = insert_node_at_line(current_line.m_target);
                current_node->m_successors.push_back(target_node);

                following_node = insert_node_at_line(next_line.m_location);

                if (current_line.m_instruction.opcode != Opcode::Branch) {
                    current_node->m_successors.push_back(following_node);
                }

                current_node->m_endLine = current_line.m_location;
                current_node = following_node;
            }
            else if (next_line_is_target) {
                following_node = insert_node_at_line(next_line.m_location);
                current_node->m_successors.push_back(following_node);

                current_node->m_endLine = current_line.m_location;
                current_node = following_node;
            } 
        }
    }

    [[nodiscard]] const ControlFlowNode* ControlFlowGraph::get_node_containing_line(const u32 line) const noexcept {
        for (const auto& [node_start, node] : m_nodes) {
            if (line >= node_start && line <= node.m_endLine) {
                return &node;
            }
        }
        return nullptr;
    }

    [[nodiscard]] ControlFlowNode* ControlFlowGraph::insert_node_at_line(const u32 start_line) noexcept {
        if (!m_nodes.contains(start_line)) {
            return &(m_nodes[start_line] = ControlFlowNode(start_line));
        }
        return &m_nodes.at(start_line);
    }

    void ControlFlowGraph::write_to_txt_file(const std::string& path) const noexcept {
        std::ofstream graph_file(path);

        if (!graph_file.is_open()) {
            std::cerr << "couldn't open out graph file " << path << '\n';
        }
        graph_file << "#nodes\n";
        for (const auto& [node_start, node] : m_nodes) {
            graph_file << node_start << ' ';
            for (const auto& line : node.m_lines) {
                graph_file << line.m_text << ';';
            }
            graph_file << '\n';
        }

        graph_file << "#edges\n";
        for (const auto& [node_start, node] : m_nodes) {
            for (const auto& out : node.m_successors) {
                graph_file << node_start << ' ' << out->m_startLine << '\n';
            }
        }
    }
    static std::mutex g_graphviz_mutex;

    void ControlFlowGraph::write_image(const std::string& path) const noexcept {
        GVC_t* gvc = gvContext();
        Agraph_t* g = agopen((char*)"G", Agdirected, nullptr);
        Agnode_t* current_node;
        std::lock_guard<std::mutex> lock(g_graphviz_mutex);
        u32 max_node = 0;
        std::map<u32, Agnode_t*> node_map{};
        for (const auto& [node_start, node] : m_nodes) {
            std::string name = std::to_string(node_start);
            current_node = agnode(g, name.data(), 1);
            node_map[node_start] = current_node;
            if (node_start > max_node) {
                max_node = node_start;
            }
            const std::string node_html_label = node.get_label_html();

            agsafeset_html(current_node, const_cast<char*>("label"), node_html_label.c_str(), "");

            agsafeset(current_node, const_cast<char*>("fontcolor"), "#8ADCFE", "");
            agsafeset(current_node, const_cast<char*>("shape"), "plaintext", "");
            agsafeset(current_node, const_cast<char*>("color"), "#8ADCFE", "");
        }

        for (const auto& [node_start, node] : m_nodes) {

            const b8 is_conditional = node.m_lines.back().m_instruction.opcode == BranchIf || node.m_lines.back().m_instruction.opcode == BranchIfNot;

            for (const auto& next : node.m_successors) {
                Agedge_t* edge = agedge(g, node_map.at(node_start), node_map.at(next->m_startLine), const_cast<char*>(""), 1);
                constexpr const char* conditional_true_color = "green";
                constexpr const char* conditional_false_color = "red";
                constexpr const char* fallthrough_color = "#8ADCFE";
                constexpr const char* branch_color = "blue";
                constexpr const char* loop_upwards_color = "purple";

                if (node.m_endLine + 1 == next->m_startLine) {
                    if (is_conditional) {
                        agsafeset(edge, const_cast<char*>("color"), conditional_false_color, "");
                    } else {
                        agsafeset(edge, const_cast<char*>("color"), fallthrough_color, "");
                    }
                }
                else if (next->m_startLine < node_start) {
                    agsafeset(edge, const_cast<char*>("color"), loop_upwards_color, "");
                }
                else if (node.m_lines.back().m_instruction.opcode == Opcode::Branch) {
                    agsafeset(edge, const_cast<char*>("color"), branch_color, "");
                }
                else {
                    agsafeset(edge, const_cast<char*>("color"), conditional_true_color, "");
                }
            }
        }
        Agraph_t* returng = agsubg(g, const_cast<char*>("return"), 1);
        Agnode_t* return_node = agnode(returng, const_cast<char*>(std::to_string(max_node).c_str()), 1);
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

    [[nodiscard]] std::vector<ControlFlowLoop> ControlFlowGraph::find_loops() noexcept {
        std::vector<ControlFlowLoop> loops{};
        for (const auto& loc : m_func->m_stackFrame.m_backwardsJumpLocs) {
            const ControlFlowNode* loop_latch = get_node_containing_line(loc.m_location);
            const ControlFlowNode* loop_head = &m_nodes.at(loc.m_target);
            if (!dominates(loop_head, loop_latch)) {
                std::cout << "backwards jump is not loop\n";
            }
            loops.emplace_back(std::move(collect_loop_body(loop_head, loop_latch)), loop_head, loop_latch);
        }
        return loops;
    }

    [[nodiscard]] std::map<const ControlFlowNode*, std::vector<const ControlFlowNode*>> ControlFlowGraph::compute_predecessors() const noexcept {
        std::map<const ControlFlowNode*, std::vector<const ControlFlowNode*>> predecessors{};
        for (const auto & [node_start, node] : m_nodes) {
            for (const auto &successor : node.m_successors) {
                predecessors[successor].push_back(&node);
            }
        }
        return predecessors;
    }


    [[nodiscard]] b8 ControlFlowGraph::dominee_not_found_outside_dominator_path(const ControlFlowNode *current_head, const ControlFlowNode *dominator, const ControlFlowNode *dominee, std::set<const ControlFlowNode*> &visited) noexcept {
        std::cout << " checking node " << current_head->m_startLine << '\n';
        if (current_head == dominator) {
            return true;
        } 
        if (current_head == dominee) {
            return false;
        }
        visited.insert(current_head);
        for (const auto& successor : current_head->m_successors) {
            if (!dominee_not_found_outside_dominator_path(successor, dominator, dominee, visited)) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] b8 ControlFlowGraph::dominates(const ControlFlowNode *dominator, const ControlFlowNode *dominee) const noexcept {
        if (dominator == dominee) {
            return true;
        }
        std::set<const ControlFlowNode*> visited;
        return dominee_not_found_outside_dominator_path(&m_nodes.at(0), dominator, dominee, visited);
    }

    static void add_successors(std::vector<const ControlFlowNode*> &nodes, const ControlFlowNode *node, const ControlFlowNode *stop) {
        if (node == stop) {
            return;
        }
        nodes.insert(nodes.end(), node->m_successors.begin(), node->m_successors.end());
        for (const auto& successor : node->m_successors) {
            add_successors(nodes, successor, stop);
        }
    }

    [[nodiscard]] std::vector<const ControlFlowNode*> ControlFlowGraph::collect_loop_body(const ControlFlowNode* head, const ControlFlowNode* latch) const noexcept {
        std::vector<const ControlFlowNode*> body = {head, latch};

        for (const auto& successor : head->m_successors) {
            add_successors(body, successor, latch);
        }

        return body;
    }
}