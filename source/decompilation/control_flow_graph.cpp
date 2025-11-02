#ifdef WIN32
#define GVDLL
#endif

#include "decompilation/control_flow_graph.h"

#include <fstream>
#include <iostream>
#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>
#include <sstream>
#include <mutex>
#include <set>
#include <algorithm>
#include <chrono>
#include <functional>

namespace dconstruct {

    constexpr const char* background_color = "#0F0F0F";
    constexpr const char* accent_color = "#8ab4feff";
    constexpr const char* conditional_true_color = "green";
    constexpr const char* conditional_false_color = "red";
    constexpr const char* fallthrough_color = accent_color;
    constexpr const char* branch_color = "blue";
    constexpr const char* loop_upwards_color = "purple";

    [[nodiscard]] opt_ref<const control_flow_loop> ControlFlowGraph::get_loop_with_head(const node_id node) const {
        for (const auto& loop : m_loops) {
            if (loop.m_headNode == node) {
                return loop;
            }
        }
        return std::nullopt;
    }

    static std::string html_escape(const std::string& input) {
        std::string output;
        output.reserve(input.size());
        for (char c : input) {
            if (c < '\n') {
                output += "borked";
                continue;
            }
            switch (c) {
            case '&':  output += "&amp;";  break;
            case '<':  output += "&lt;";   break;
            case '>':  output += "&gt;";   break;
            case '"':  output += "&quot;"; break;
            case '\'': output += "&#39;";  break;
            default:   output += c;        break;
            }
        }
        return output;
    }

    [[nodiscard]] std::string control_flow_node::get_label_html() const {
        std::stringstream ss;

        ss << R"(<TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" CELLPADDING="12">"
        "<TR><TD ALIGN="LEFT" BALIGN="LEFT"><FONT FACE="Consolas">)";

        ss << std::hex << "idx: " << m_index << "  ipdom: " << m_ipdom << "  postorder: " << m_postorder << std::dec << "<BR/>";


        for (const auto& line : m_lines) {
            ss << line.m_text << " " << html_escape(line.m_comment) << "&#160;&#160;<BR/>";
        }

        ss << "</FONT></TD></TR></TABLE>";


        return ss.str();
    }

    [[nodiscard]] bool control_flow_node::operator==(const control_flow_node& rhs) const noexcept {
        if (m_lines.size() != rhs.m_lines.size()) {
            return false;
        }
        for (u32 i = 0; i < m_lines.size(); ++i) {
            if (m_lines[i].m_instruction != rhs.m_lines[i].m_instruction) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] bool control_flow_node::operator!=(const control_flow_node& rhs) const noexcept {
        return !(*this == rhs);
    }

    void control_flow_node::determine_register_nature() {
        for (const auto& line : m_lines) {
            const auto& istr = line.m_instruction;

            if (istr.destination == istr.operand1 && istr.op1_is_reg() && !m_writtenToRegs[istr.destination])  {
                m_readFirstTwiceRegs[istr.destination] = m_readFirstTwiceRegs[istr.destination] || m_readFirstRegs[istr.destination] && !m_writtenToRegs[istr.destination];
                m_readFirstRegs.set(istr.destination);
                continue;
            }
            
            if (!istr.destination_is_immediate() && istr.destination < ARGUMENT_REGISTERS_IDX) {
                m_writtenToRegs.set(istr.destination);
            }

            if (istr.op1_is_reg() && !m_writtenToRegs[istr.operand1]) {
                m_readFirstTwiceRegs[istr.operand1] = m_readFirstTwiceRegs[istr.operand1] || m_readFirstRegs[istr.operand1] && !m_writtenToRegs[istr.operand1];
                m_readFirstRegs.set(istr.operand1);
            }
            if (istr.op2_is_reg() && !m_writtenToRegs[istr.operand2]) {
                m_readFirstTwiceRegs[istr.operand2] = m_readFirstTwiceRegs[istr.operand2] || m_readFirstRegs[istr.operand2] && !m_writtenToRegs[istr.operand2];
                m_readFirstRegs.set(istr.operand2);
            }
        }
    }

    [[nodiscard]] std::pair<reg_set, reg_set> control_flow_node::get_read_count_starting_at(const istr_line start_line) const noexcept {
        reg_set read_first, multi_read;
        reg_set write_regs;
        for (istr_line i = start_line; i < m_lines.size(); ++i) {
            const auto& istr = m_lines[i].m_instruction;

            if (istr.destination == istr.operand1 && istr.op1_is_reg() && !write_regs[istr.destination])  {
                multi_read[istr.destination] = multi_read[istr.destination] || read_first[istr.destination] && !write_regs[istr.destination];
                read_first.set(istr.destination);
                write_regs.set(istr.destination);
                continue;
            }
            
            if (!istr.destination_is_immediate() && istr.destination < ARGUMENT_REGISTERS_IDX) {
                write_regs.set(istr.destination);
            }

            if (istr.op1_is_reg() && !write_regs[istr.operand1]) {
                multi_read[istr.operand1] = multi_read[istr.operand1] || read_first[istr.operand1] && !write_regs[istr.operand1];
                read_first.set(istr.operand1);
            }
            if (istr.op2_is_reg() && !write_regs[istr.operand2]) {
                multi_read[istr.operand2] = multi_read[istr.operand2] || read_first[istr.operand2] && !write_regs[istr.operand2];
                read_first.set(istr.operand2);
            }
        }

        return {read_first, multi_read};
    }

    static void insert_node_at_line(const istr_line start_line, std::map<node_id, control_flow_node>& nodes) {
        if (!nodes.contains(start_line)) {
            nodes.emplace(start_line, start_line);
        }
    }

    [[nodiscard]] std::vector<node_id> postorder(const std::vector<control_flow_node>& nodes) {
        std::vector<node_id> result;
        const u32 size = nodes.size();
        result.reserve(size);
        node_set visited(size, false);

        std::vector<std::pair<node_id, size_t>> stack;
        stack.emplace_back(nodes.back().m_index, 0);
        u32 i = 0;
        while (!stack.empty()) {
            auto [n, i] = stack.back();
            if (!visited[n]) {
                visited[n] = true;
            }

            const auto& preds = nodes[n].m_predecessors;
            if (i < preds.size()) {
                stack.back().second++;
                const auto p = preds[i];
                if (!visited[p]) {
                    stack.emplace_back(p, 0);
                }
            }
            else {
                result.push_back(n);
                stack.pop_back();
            }
        }
        return result;
    }

    //recursive version of the above
    /*[[nodiscard]] std::vector<node_id> create_postord_recursive(const std::vector<control_flow_node>& nodes) {
        std::vector<node_id> result;
        const u32 size = nodes.size();
        result.reserve(size);
        node_set visited(size, false);

        std::function<void(node_id)> visit = [&](node_id n) {
            if (visited[n]) {
                return;
            }
            visited[n] = true;
            for (const auto& pred : nodes[n].m_predecessors) {
                visit(pred);
            }
            result.push_back(n);
        };

        visit(nodes.back().m_index);
        std::reverse(result.begin(), result.end());
        return result;
    }*/

    ControlFlowGraph::ControlFlowGraph(const function_disassembly &func) : m_func(func)  {
        const std::vector<u32> &labels = func.m_stackFrame.m_labels;
        std::map<node_id, control_flow_node> nodes;
        nodes.emplace(0, 0);
        node_id current_node = 0;
        node_id following_node;
        for (u32 i = 0; i < func.m_lines.size(); ++i) {
            const function_disassembly_line &current_line = func.m_lines[i];
            nodes[current_node].m_lines.push_back(current_line);
            if (i == func.m_lines.size() - 1) {
                nodes[current_node].m_endLine = current_line.m_location;
                break;
            }
            const function_disassembly_line &next_line = func.m_lines[i + 1];

            bool next_line_is_target = std::find(labels.begin(), labels.end(), next_line.m_location) != labels.end();

            if (current_line.m_target != -1) {
                insert_node_at_line(current_line.m_target, nodes);
                nodes[current_node].m_targetNode = current_line.m_target;
                nodes[current_line.m_target].m_predecessors.push_back(current_node);

                following_node = next_line.m_location;

                insert_node_at_line(following_node, nodes);

                if (current_line.m_instruction.opcode != Opcode::Branch) {
                    nodes[current_node].m_followingNode = following_node;
                    nodes[following_node].m_predecessors.push_back(current_node);
                }

                nodes[current_node].m_endLine = current_line.m_location;
                current_node = following_node;
            }
            else if (next_line_is_target) {
                following_node = next_line.m_location;
                insert_node_at_line(following_node, nodes);
                nodes[current_node].m_followingNode = following_node;
                nodes[following_node].m_predecessors.push_back(current_node);
                nodes[current_node].m_endLine = current_line.m_location;
                current_node = following_node;
            } 
        }
        m_nodes.resize(nodes.size());
        std::unordered_map<node_id, node_id> nodes_to_index;
        u16 i = 0;
        for (auto& [id, node] : nodes) {
            nodes_to_index[id] = i;
            node.m_index = i++;
            node.m_followingNode = node.m_followingNode ? i : 0;
        }
        for (auto& [id, node] : nodes) {
            node.m_targetNode = nodes_to_index.at(node.m_targetNode);
            for (auto& pred : node.m_predecessors) {
                pred = nodes_to_index.at(pred);
            }
            m_nodes[node.m_index] = node;
            m_nodes[node.m_index].determine_register_nature();
        }
        auto& last_node = m_nodes.back();
        const u8 last_dest = last_node.m_lines.back().m_instruction.destination;
        const bool flag = !m_func.m_isScriptFunction && !last_node.m_writtenToRegs[last_dest];
        last_node.m_readFirstRegs[last_dest] = last_node.m_readFirstRegs[last_dest] || flag;
        compute_postdominators();
        find_loops();
    }

    void ControlFlowGraph::write_to_txt_file(const std::string& path) const {
        std::ofstream graph_file(path);

        if (!graph_file.is_open()) {
            std::cerr << "couldn't open out graph file " << path << '\n';
        }
        graph_file << "#nodes\n";
        for (const auto& node : m_nodes) {
            graph_file << node.m_startLine << '/' << node.m_index << ' ';
            for (const auto& line : node.m_lines) {
                graph_file << line.m_text << ';';
            }
            graph_file << '\n';
        }

        graph_file << "#edges\n";
        for (const auto& node : m_nodes) {
            if (node.m_followingNode) {
                graph_file << node.m_startLine << '/' << node.m_index << ' ' << node.m_followingNode << '\n';
            }
            if (node.m_targetNode) {
                graph_file << node.m_startLine << '/' << node.m_index << ' ' << node.m_targetNode << '\n';
            }
        }
    }
    static std::mutex g_graphviz_mutex;

    void ControlFlowGraph::write_image(const std::string& path) const {
        GVC_t* gvc = gvContext();
        Agraph_t* g = agopen((char*)"G", Agdirected, nullptr);
        std::lock_guard<std::mutex> lock(g_graphviz_mutex);
        
        const auto graph_nodes = insert_graphviz_nodes(g);

        insert_graphviz_edges(g, graph_nodes);

        Agraph_t* returng = agsubg(g, const_cast<char*>("return"), 1);
        Agnode_t* return_node = agnode(returng, const_cast<char*>(std::to_string(m_nodes.back().m_index).c_str()), 1);

        insert_loop_subgraphs(g);

        agsafeset(return_node, const_cast<char*>("peripheries"), "1", "");
        agsafeset(returng, const_cast<char*>("rank"), "max", "");
        agsafeset(g, const_cast<char*>("bgcolor"), background_color, const_cast<char*>(""));
        agsafeset(g, const_cast<char*>("splines"), "ortho", const_cast<char*>(""));
        gvLayout(gvc, g, "dot");
        gvRenderFilename(gvc, g, "svg", path.c_str());
        gvFreeLayout(gvc, g);
        agclose(g);
        gvFreeContext(gvc);
    }

    void ControlFlowGraph::insert_loop_subgraphs(Agraph_t *g) const {
        for (u32 i = 0; i < m_loops.size(); ++i) {
            const std::string loop_name = "cluster_loop_" + std::to_string(i);
            Agraph_t *loopg = agsubg(g, const_cast<char *>(loop_name.c_str()), 1);

            Agraph_t* loopheadg = agsubg(loopg, const_cast<char*>("head"), 1);
            agnode(loopheadg, const_cast<char*>(std::to_string(m_loops[i].m_headNode).c_str()), 1);

            Agraph_t* looplatchg = agsubg(loopg, const_cast<char*>("latch"), 1);
            agnode(looplatchg, const_cast<char*>(std::to_string(m_loops[i].m_latchNode).c_str()), 1);

            for (const auto &loop_node : m_loops[i].m_body) {
                std::string name = std::to_string(loop_node);
                agnode(loopg, name.data(), 1);
            }
            agsafeset(loopheadg, const_cast<char*>("rank"), "source", "");
            agsafeset(looplatchg, const_cast<char*>("rank"), "max", "");
            agsafeset(loopg, const_cast<char*>("label"), loop_name.c_str(), "");
            agsafeset(loopg, const_cast<char*>("fontcolor"), accent_color, "");
            agsafeset(loopg, const_cast<char*>("fontname"), "Consolas", "");
            agsafeset(loopg, const_cast<char*>("color"), "purple", "");
            agsafeset(loopg, const_cast<char*>("style"), "dashed", "");
        }
    }

    [[nodiscard]] std::vector<Agnode_t*> ControlFlowGraph::insert_graphviz_nodes(Agraph_t *g) const {
        std::vector<Agnode_t*> ag_nodes;
        ag_nodes.reserve(m_nodes.size());
        for (const auto& node : m_nodes) {
            std::string name = std::to_string(node.m_index);
            Agnode_t* current_node = agnode(g, name.data(), 1);
            ag_nodes.push_back(current_node);
            const std::string node_html_label = node.get_label_html();

            agsafeset_html(current_node, const_cast<char*>("label"), node_html_label.c_str(), "");

            agsafeset(current_node, const_cast<char*>("fontcolor"), accent_color, "");
            agsafeset(current_node, const_cast<char*>("shape"), "plaintext", "");
            agsafeset(current_node, const_cast<char*>("color"), accent_color, "");
        }
        return ag_nodes;
    }

    void ControlFlowGraph::insert_graphviz_edges(Agraph_t* g, const std::vector<Agnode_t*>& graph_nodes) const {
        for (const auto& node : m_nodes) {
            const auto node_start = node.m_index;
            const bool is_conditional = node.m_lines.back().m_instruction.opcode == Opcode::BranchIf || node.m_lines.back().m_instruction.opcode == Opcode::BranchIfNot;

            if (node.m_followingNode) {
                Agedge_t* edge = agedge(g, graph_nodes[node_start], graph_nodes[node.m_followingNode], const_cast<char*>(""), 1);
                agsafeset(edge, const_cast<char*>("color"), is_conditional ? conditional_false_color : fallthrough_color, "");
            }
            if (node.m_targetNode) {
                Agedge_t* edge = agedge(g, graph_nodes[node_start], graph_nodes[node.m_targetNode], const_cast<char*>(""), 1);
                if (node_start > node.m_targetNode) {
                    agsafeset(edge, const_cast<char*>("color"), loop_upwards_color, "");
                } else if (node.m_lines.back().m_instruction.opcode == Opcode::Branch) {
                    agsafeset(edge, const_cast<char*>("color"), branch_color, "");
                } else {
                    agsafeset(edge, const_cast<char*>("color"), conditional_true_color, "");
                }
            }
        }
    }

    void ControlFlowGraph::find_loops() {
        for (const auto& loc : m_func.m_stackFrame.m_backwardsJumpLocs) {
            const node_id loop_head = std::lower_bound(
                m_nodes.begin(), 
                m_nodes.end(), 
                loc.m_target, 
                [](const control_flow_node& node, const u64 target) -> bool { 
                    return node.m_startLine < target;
                }
            )->m_index;
            const node_id loop_latch = std::lower_bound(
                m_nodes.begin(), 
                m_nodes.end(), 
                loc.m_location, 
                [](const control_flow_node& node, const u64 target) -> bool {
                    return node.m_startLine < target;
                }
            )->m_index - 1;
            m_loops.emplace_back(std::move(collect_loop_body(loop_head, loop_latch)), loop_head, loop_latch);
        }
    }

    [[nodiscard]] const control_flow_node& intersect(const node_id node_b1, const node_id node_b2, const std::vector<control_flow_node>& nodes) {
        const control_flow_node* b1 = &nodes[node_b1];
        const control_flow_node* b2 = &nodes[node_b2];
        while (b1->m_index != b2->m_index) {
            while (b1->m_postorder < b2->m_postorder) {
                b1 = &nodes[b1->m_ipdom];
            }
            while (b2->m_postorder < b1->m_postorder) {
                b2 = &nodes[b2->m_ipdom];
            }
        }
        return *b1;
    }

    void ControlFlowGraph::compute_postdominators() {

        auto order = postorder(m_nodes);

        static constexpr node_id UNDEF = std::numeric_limits<node_id>::max();

        for (u32 i = 0; i < m_nodes.size(); ++i) {
            m_nodes[order[i]].m_postorder = i;
        }
        order.pop_back();
		std::reverse(order.begin(), order.end());

        const u32 N = order.size();
        std::unordered_map<node_id, node_id> ipdom;
        for (auto n : order) {
            ipdom[n] = UNDEF;
        }

        ipdom[m_nodes.back().m_index] = m_nodes.back().m_index;
        m_nodes.back().m_ipdom = m_nodes.back().m_index;

        bool changed = true;
        while (changed) {
            changed = false;
            for (u32 i = 0; i < N; ++i) {
                node_id n = order[i];
                node_id new_ipdom = UNDEF;
                const control_flow_node& node = m_nodes.at(n);
                const auto dir_s = node.m_followingNode;
                const auto tar_s = node.m_targetNode;

                if (dir_s && ipdom.at(dir_s) != UNDEF) {
                    new_ipdom = dir_s;
                } else if (tar_s && ipdom.at(tar_s) != UNDEF) {
                    new_ipdom = tar_s;
                }
                if (new_ipdom == UNDEF) {
                    continue;
                }
                
                if (dir_s && ipdom.at(dir_s) != UNDEF && dir_s != new_ipdom) {
                    new_ipdom = intersect(dir_s, new_ipdom, m_nodes).m_index;
                }
                if (tar_s && ipdom.at(tar_s) != UNDEF && tar_s != new_ipdom) {
                    new_ipdom = intersect(tar_s, new_ipdom, m_nodes).m_index;
                }

                if (ipdom.at(n) != new_ipdom) {
                    ipdom[n] = new_ipdom;
                    m_nodes.at(n).m_ipdom = new_ipdom;
                    changed = true;
                }
            }
        }
    }

    [[nodiscard]] std::vector<node_id> ControlFlowGraph::collect_loop_body(const node_id head, const node_id latch) const {
        std::vector<node_id> body;
        for (node_id i = head; i < latch; ++i) {
            body.push_back(i);
        }
        return body;
    }


    // void ControlFlowGraph::get_register_nature(
    //     const control_flow_node& start_node,
    //     reg_set check_regs,
    //     reg_set& read_first, 
    //     const node_id stop_node,
    //     node_set& asd,
    //     const istr_line start_line
    // ) const noexcept {

    //     struct frame {
    //         const control_flow_node* node;
    //         reg_set regs;
    //     };

    //     std::vector<std::pair<const control_flow_node&, reg_set>> node_stack;
    //     node_set visited;
    //     visited.resize(m_nodes.size(), false);

    //     node_stack.push_back({&start_node, check_regs});

    //     while (!node_stack.empty()) {
    //         auto [current_node, check_write_regs] = node_stack.back();
    //         node_stack.pop_back();
    //         if (visited[current_node->m_index]) {
    //             continue;
    //         }
    //         visited[current_node->m_index] = true;
    //         for (u32 i = start_line; i < current_node->m_lines.size(); ++i) {
    //             const function_disassembly_line& line = current_node->m_lines[i];
    //             if (check_write_regs.to_ullong() == 0) {
    //                 break;
    //             }
    //             const Instruction& istr = line.m_instruction;
    //             if (istr.opcode == Opcode::Return && check_write_regs[istr.destination]) {
    //                 check_write_regs.reset(istr.destination);
    //                 if (!m_func.m_isScriptFunction) {
    //                     read_first.set(istr.destination);
    //                 }
    //                 break;
    //             }
    //             if (istr.destination == istr.operand1 && !istr.operand1_is_immediate() && check_write_regs[istr.destination]) {
    //                 check_write_regs.reset(istr.destination);
    //                 read_first.set(istr.destination);
	// 				check_regs.reset(istr.destination);
    //                 continue;
    //             }
    //             if (!istr.destination_is_immediate() && istr.destination < ARGUMENT_REGISTERS_IDX && check_write_regs[istr.destination]) {
    //                 check_write_regs.reset(istr.destination);
    //             }
    //             if (istr.op1_is_reg() && istr.operand1 < ARGUMENT_REGISTERS_IDX && check_write_regs[istr.operand1]) {
    //                 check_write_regs.reset(istr.operand1);
    //                 check_regs.reset(istr.operand1);
    //                 read_first.set(istr.operand1);
    //             }
    //             if (istr.op2_is_reg() && istr.operand2 < ARGUMENT_REGISTERS_IDX && check_write_regs[istr.operand2]) {
    //                 check_write_regs.reset(istr.operand2);
    //                 check_regs.reset(istr.operand1);
    //                 read_first.set(istr.operand2);
    //             }
    //         }
    //         use_start_line = false;
    //         if (check_write_regs.to_ullong() == 0) {
    //             continue;
    //         }
    //         if (check_regs.to_ullong() == 0) {
    //             return;
    //         }
    //         if (current_node->m_followingNode && current_node->m_followingNode != stop_node) {
    //             node_stack.push_back({&m_nodes.at(current_node->m_followingNode), check_write_regs });
    //         }
    //         if (current_node->m_targetNode && current_node->m_targetNode != stop_node) {
    //             node_stack.push_back({&m_nodes.at(current_node->m_targetNode), check_write_regs });
    //         }
    //     }
    // }

    [[nodiscard]] reg_set ControlFlowGraph::get_register_nature(
        const control_flow_node& start_node,
        reg_set check_regs,
        const node_id stop_node,
        const istr_line start_line)
    const noexcept {
        reg_set result;
        node_set checked(m_nodes.size(), false);
        std::vector<std::reference_wrapper<const control_flow_node>> node_stack;

        if (start_line) {
            const auto [read_first, _] = start_node.get_read_count_starting_at(start_line);
            result |= read_first & check_regs;
            check_regs &= ~read_first;
            checked[start_node.m_index] = true;
            if (start_node.m_followingNode) {
                node_stack.push_back(m_nodes[start_node.m_followingNode]);
            }
            if (start_node.m_targetNode) {
                node_stack.push_back(m_nodes[start_node.m_targetNode]);
            }
        } else {
            node_stack.push_back(start_node);
        }

        while (!node_stack.empty() && check_regs.to_ullong() != 0) {
            const auto& node = node_stack.back().get();
            node_stack.pop_back();
            checked[node.m_index] = true;

            result |= node.m_readFirstRegs;
            check_regs &= ~node.m_readFirstRegs;

            if (node.m_followingNode && !checked[node.m_followingNode] && node.m_followingNode != stop_node) {
                node_stack.push_back(m_nodes[node.m_followingNode]);
            }
            if (node.m_targetNode && !checked[node.m_targetNode] && node.m_targetNode != stop_node) {
                node_stack.push_back(m_nodes[node.m_targetNode]);
            }
        }

        return result;
    }

   /* u16 ControlFlowGraph::get_register_read_count(
        const control_flow_node& start_node,
        const reg_idx reg_to_check,
        const node_id stop_node,
        node_set& checked,
        const istr_line start_line
    ) const noexcept {
        if (checked[start_node.m_index]) {
            return 0;
        }
        checked[start_node.m_index] = true;
        u16 count = 0;
        for (u32 i = start_line; i < start_node.m_lines.size(); ++i) {
            if (count >= 2) {
                return count;
            }
            const function_disassembly_line& line = start_node.m_lines[i];
            const Instruction& istr = line.m_instruction;
            if (reg_to_check == istr.destination && istr.opcode == Opcode::Return) {
                if (!m_func.m_isScriptFunction) {
                    return ++count;
                }
                return count;
            }
            if (reg_to_check == istr.destination && istr.destination == istr.operand1 && istr.op1_is_reg()) {
                return ++count;
            }
            if (reg_to_check == istr.destination && !istr.destination_is_immediate()) {
                return count;
            }
            if (reg_to_check == istr.operand1 && istr.op1_is_reg()) {
                count++;
            }
            if (reg_to_check == istr.operand2 && istr.op2_is_reg()) {
                count++;
            }
        }
        if (start_node.m_index == stop_node) {
            return count;
        }
        if (start_node.m_followingNode) {
            count += get_register_read_count(m_nodes.at(start_node.m_followingNode), reg_to_check, stop_node, checked);
        }
        if (count > 2) {
            return count;
        }
        if (start_node.m_targetNode) {
            count += get_register_read_count(m_nodes.at(start_node.m_targetNode), reg_to_check, stop_node, checked);
        }
        return count;
    }*/

    u8 ControlFlowGraph::get_register_read_count(const control_flow_node& start_node,
        const reg_idx reg_to_check,
        const node_id stop_node,
        const istr_line start_line
    ) const noexcept {
        //reg_set single, multi;
       // reg_set check_reg = reg_set().set(reg_to_check);
        
        node_set checked(m_nodes.size(), false);
        bool read_once = false;
        std::vector<std::reference_wrapper<const control_flow_node>> node_stack;

        if (start_line) {
            const auto [single_read, multi_read] = start_node.get_read_count_starting_at(start_line);

            
            if (multi_read[reg_to_check]) {
                return 2;
            }

            read_once = single_read[reg_to_check];

            checked[start_node.m_index] = true;
            if (start_node.m_followingNode) {
                node_stack.push_back(m_nodes[start_node.m_followingNode]);
            }
            if (start_node.m_targetNode) {
                node_stack.push_back(m_nodes[start_node.m_targetNode]);
            }
        }
        else {
            node_stack.push_back(start_node);
        }

        while (!node_stack.empty()) {
            const auto& node = node_stack.back().get();
            node_stack.pop_back();
            checked[node.m_index] = true;


            // if we read first twice, than we can return 2
            // or if we already read first at the previous node, that node doesnt write to the check reg, and we read first at the current node
            if (node.m_readFirstTwiceRegs[reg_to_check] || (node.m_readFirstRegs[reg_to_check] && read_once)) {
                return 2;
            }
            
            // if the current node reads AND writes, its 1
            if (node.m_readFirstRegs[reg_to_check] && node.m_writtenToRegs[reg_to_check]) {
                
            }

            if (node.m_followingNode && !checked[node.m_followingNode] && node.m_followingNode != stop_node) {
                node_stack.push_back(m_nodes[node.m_followingNode]);
            }
            if (node.m_targetNode && !checked[node.m_targetNode] && node.m_targetNode != stop_node) {
                node_stack.push_back(m_nodes[node.m_targetNode]);
            }
        }

		return single[reg_to_check] ? (multi[reg_to_check] ? 2 : 1) : 0;
    }

    [[nodiscard]] reg_set ControlFlowGraph::get_registers_written_to(const control_flow_node& start_node, const node_id stop) const {
        reg_set result;
        node_set checked(m_nodes.size(), false);
        std::vector<std::reference_wrapper<const control_flow_node>> node_stack = {start_node};
        
        while (!node_stack.empty()) {
            const auto& current_node = node_stack.back().get();
            node_stack.pop_back();
            checked[current_node.m_index] = true;
            result |= current_node.m_writtenToRegs;
            
            if (current_node.m_followingNode && !checked[current_node.m_followingNode] && current_node.m_followingNode != stop) {
                node_stack.push_back(m_nodes[current_node.m_followingNode]);
            }
            if (current_node.m_targetNode && !checked[current_node.m_targetNode] && current_node.m_targetNode != stop) {
                node_stack.push_back(m_nodes[current_node.m_targetNode]);
            }
        }

        return result;
    }
    
    [[nodiscard]] reg_set ControlFlowGraph::get_branch_phi_registers(const control_flow_node& start_node) const noexcept {
        reg_set read_first_branches, read_first_ipdom, result;
        node_set checked(m_nodes.size(), false);
        node_id ipdom = start_node.m_ipdom;

        if (ipdom == start_node.m_index) {
            result.set(start_node.m_targetNode);
            return result;
        }

        const reg_set left = get_registers_written_to(m_nodes[start_node.m_followingNode], ipdom);
        const reg_set right = get_registers_written_to(m_nodes[start_node.m_targetNode], ipdom);

        const reg_set check_regs = left | right;
        const reg_set res = get_register_nature(m_nodes.at(ipdom), check_regs, m_nodes.back().m_index + 1);

        return res;
    }

    [[nodiscard]] reg_set ControlFlowGraph::get_loop_phi_registers(const control_flow_node& head_node) const noexcept {
        node_set checked(m_nodes.size(), false);

        const reg_set written = head_node.m_writtenToRegs | get_registers_written_to(m_nodes[head_node.m_followingNode], head_node.m_index);
        const reg_set read_first = get_register_nature(m_nodes[head_node.m_targetNode], written, m_nodes.back().m_index);

        return read_first;
    }

    [[nodiscard]] const control_flow_node& ControlFlowGraph::get_final_loop_condition_node(const control_flow_loop& loop, const node_id exit_node) const noexcept {
        for (u16 i = loop.m_latchNode; i > loop.m_headNode; --i) {
            if (m_nodes[i].m_targetNode == exit_node) {
                return m_nodes[i];
            }
        }
        return m_nodes[loop.m_headNode];
    }
}