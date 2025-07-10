#define GVDLL

#include "control_flow_graph.h"

#include <fstream>
#include <iostream>
#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>
#include <sstream>
#include <set>
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
        m_nodes.push_back(std::make_unique<ControlFlowNode>(0));
        ControlFlowNode *current_node = m_nodes[0].get();
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

    const ControlFlowNode* ControlFlowGraph::get_node_at_line(const u32 start_line) const noexcept {
        for (const auto& node : m_nodes) {
            if (node->m_startLine == start_line) {
                return node.get();
            }
        }
        return nullptr;
    }

    const ControlFlowNode* ControlFlowGraph::get_node_containing_line(const u32 line) const noexcept {
        for (const auto& node : m_nodes) {
            if (line >= node->m_startLine && line <= node->m_endLine) {
                return node.get();
            }
        }
        return nullptr;
    }

    ControlFlowNode* ControlFlowGraph::insert_node_at_line(const u32 start_line) noexcept {
        ControlFlowNode *existing = const_cast<ControlFlowNode*>(get_node_at_line(start_line));
        if (existing == nullptr) {
            m_nodes.emplace_back(std::make_unique<ControlFlowNode>(start_line));
            return m_nodes.back().get();
        }
        return existing;
    }

    void ControlFlowGraph::write_to_txt_file(const std::string& path) const noexcept {
        std::ofstream graph_file(path);

        if (!graph_file.is_open()) {
            std::cerr << "couldn't open out graph file " << path << '\n';
        }
        graph_file << "#nodes\n";
        for (const auto& node : m_nodes) {
            graph_file << node->m_startLine << ' ';
            for (const auto& line : node->m_lines) {
                graph_file << line.m_text << ';';
            }
            graph_file << '\n';
        }

        graph_file << "#edges\n";
        for (const auto& node : m_nodes) {
            for (const auto& out : node->m_successors) {
                graph_file << node->m_startLine << ' ' << out->m_startLine << '\n';
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
        std::map<const ControlFlowNode*, Agnode_t*> node_map{};
        for (const auto& node : m_nodes) {
            std::string name = std::to_string(node->m_startLine);
            current_node = agnode(g, name.data(), 1);
            node_map[node.get()] = current_node;
            if (node->m_startLine > max_node) {
                max_node = node->m_startLine;
            }
            const std::string node_html_label = node->get_label_html();

            agsafeset_html(current_node, const_cast<char*>("label"), node_html_label.c_str(), "");

            agsafeset(current_node, const_cast<char*>("fontcolor"), "#8ADCFE", "");
            agsafeset(current_node, const_cast<char*>("shape"), "plaintext", "");
            agsafeset(current_node, const_cast<char*>("color"), "#8ADCFE", "");
        }

        for (const auto& node : m_nodes) {
            for (const auto& next : node->m_successors) {
                auto test1 = node_map[node.get()];
                auto test2 = node_map[next];
                Agedge_t* edge = agedge(g, test1, test2, const_cast<char*>(""), 1);
                if (node->m_endLine + 1 == next->m_startLine) {
                    agsafeset(edge, const_cast<char*>("color"), "red", "");
                }
                else if (next->m_startLine < node->m_startLine) {
                    agsafeset(edge, const_cast<char*>("color"), "purple", "");
                }
                else if (node->m_lines.back().m_instruction.opcode == Opcode::Branch) {
                    agsafeset(edge, const_cast<char*>("color"), "red", "");
                }
                else {
                    agsafeset(edge, const_cast<char*>("color"), "green", "");
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
        for (const auto& loc : m_func->m_stackFrame.m_backwardsJumpLocs) {
            const ControlFlowNode *loop_latch = get_node_containing_line(loc.m_location);
            const ControlFlowNode *potential_loop_head = get_node_at_line(loc.m_target);
            if (dominates(potential_loop_head, loop_latch)) {
                std::cout << "yippie\n";
            }
        }

        return {};
    }

    [[nodiscard]] std::map<const ControlFlowNode*, std::vector<const ControlFlowNode*>> ControlFlowGraph::compute_predecessors() const noexcept {
        std::map<const ControlFlowNode*, std::vector<const ControlFlowNode*>> predecessors{};
        for (const auto &node : m_nodes) {
            for (const auto &successor : node->m_successors) {
                predecessors[successor].push_back(node.get());
            }
        }
        return predecessors;
    }

    b8 dfs(const ControlFlowNode *node, const ControlFlowNode *n, const ControlFlowNode *m, std::set<const ControlFlowNode*> &visited) {
        if (node == m) {
            return true;
        }
        if (node == n) {
            return false;
        }
        if (visited.contains(node)) {
            return false;
        }
        visited.insert(node);
        for (const auto &successor : node->m_successors) {
            if (dfs(successor, n, m, visited)) {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] b8 ControlFlowGraph::dominates(const ControlFlowNode *n, const ControlFlowNode *m) const noexcept {
        if (n == m) {
            return true;
        }
        std::set<const ControlFlowNode*> visited;
        return dfs(m_nodes[0].get(), n, m, visited);
    }
    // [[nodiscard]] std::vector<u32> ControlFlowGraph::get_idom_predecessors(const u32 n) noexcept {
    //     std::vector<u32> res{};
    //     for (const auto &p : m_predecessors[n]) {
    //         if (m_immediateDominators.contains(p)) {
    //             res.push_back(p);
    //         }
    //     }
    //     return res;
    // }

    // void ControlFlowGraph::find_immediate_dominators() noexcept {
    //     compute_predecessors();
    //     m_immediateDominators[0] = 0;

    //     b8 changed = true;
    //     while (changed) {
    //         changed = false;
    //         for (const auto &[n, node] : m_nodes) { 
    //             if (n == 0) continue;
    //             const std::vector<u32> &idom_preds = get_idom_predecessors(n);
    //             if (idom_preds.empty()) {
    //                 continue;
    //             }
    //             u32 new_idom = idom_preds[0];
    //             for (u32 p = 1; p < idom_preds.size(); ++p) {
    //                 new_idom = intersect(new_idom, idom_preds[p]);
    //             }
    //             if (m_immediateDominators[n] != new_idom) {
    //                 m_immediateDominators[n] = new_idom;
    //                 changed = true;
    //             }
    //         }
    //     }
    // }

    // [[nodiscard]] u32 ControlFlowGraph::intersect(const u32 node1, const u32 node2) const noexcept {
    //     u32 finger1 = node1;
    //     u32 finger2 = node2;
    //     while (finger1 != finger2) {
    //         while (finger1 < finger2) {
    //             finger1 = m_immediateDominators.at(finger1);
    //         }
    //         while (finger2 < finger1) {
    //             finger2 = m_immediateDominators.at(finger2);
    //         }
    //     }
    //     return finger1;
    // }
}