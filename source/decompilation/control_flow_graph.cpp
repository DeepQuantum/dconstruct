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

        for (const auto& line : m_lines) {
            ss << line.m_text << " " << html_escape(line.m_comment) << "&#160;&#160;<BR/>";
        }

        ss << "</FONT></TD></TR></TABLE>";


        return ss.str();
    }

    [[nodiscard]] inline node_id control_flow_node::get_direct_successor() const {
        return get_last_line().m_location + 1;
    }

    [[nodiscard]] inline const function_disassembly_line& control_flow_node::get_last_line() const {
        return m_lines.back();
    }

    [[nodiscard]] inline node_id control_flow_node::get_adjusted_target() const {
        return m_lines.back().m_target;
    }

    [[nodiscard]] b8 control_flow_node::operator==(const control_flow_node& rhs) const noexcept {
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

    [[nodiscard]] b8 control_flow_node::operator!=(const control_flow_node& rhs) const noexcept {
        return !(*this == rhs);
    }

    void control_flow_node::determine_register_nature() {
        for (const auto& line : m_lines) {
            const auto& istr = line.m_instruction;
            if (!istr.destination_is_immediate()) {
                m_writtenToRegs.set(istr.destination);
            }

            if (istr.op1_is_reg() && !m_writtenToRegs.test(istr.operand1)) {
                m_readFirstRegs.set(istr.operand1);
            }
            if (istr.destination == istr.operand1 && istr.op1_is_reg()) {
                m_readFirstRegs.set(istr.operand1);
                continue;
            }
            if (!istr.destination_is_immediate() && !m_writtenToRegs.test(istr.destination)) {
                m_readFirstRegs.set(istr.destination);
            }
            if (istr.op1_is_reg() && !m_writtenToRegs.test(istr.operand1)) {
                m_readFirstRegs.set(istr.operand1);
            }
            if (istr.op2_is_reg() && !m_writtenToRegs.test(istr.operand2)) {
                m_readFirstRegs.set(istr.operand2);
            }
        }
    }

    ControlFlowGraph::ControlFlowGraph(const function_disassembly *func) : m_func(func)  {
        const std::vector<u32> &labels = func->m_stackFrame.m_labels;
        m_nodes.emplace(0, 0);
        node_id current_node = 0;
        node_id following_node;
        for (u32 i = 0; i < func->m_lines.size(); ++i) {
            const function_disassembly_line &current_line = func->m_lines[i];
            m_nodes[current_node].m_lines.push_back(current_line);
            if (i == func->m_lines.size() - 1) {
                m_nodes[current_node].m_endLine = current_line.m_location;
                m_returnNode = current_node;
                break;
            }
            const function_disassembly_line &next_line = func->m_lines[i + 1];

            b8 next_line_is_target = std::find(labels.begin(), labels.end(), next_line.m_location) != labels.end();

            if (current_line.m_target != -1) {
                insert_node_at_line(current_line.m_target);
                m_nodes[current_node].m_successors.push_back(current_line.m_target);
                m_nodes[current_line.m_target].m_predecessors.push_back(current_node);

                following_node = next_line.m_location;

                insert_node_at_line(following_node);

                if (current_line.m_instruction.opcode != Opcode::Branch) {
                    m_nodes[current_node].m_successors.push_back(following_node);
                    m_nodes[following_node].m_predecessors.push_back(current_node);
                }

                m_nodes[current_node].m_endLine = current_line.m_location;
                current_node = following_node;
            }
            else if (next_line_is_target) {
                following_node = next_line.m_location;
                insert_node_at_line(following_node);
                m_nodes[current_node].m_successors.push_back(following_node);
                m_nodes[following_node].m_predecessors.push_back(current_node);
                m_nodes[current_node].m_endLine = current_line.m_location;
                current_node = following_node;
            } 
        }
        /*for (auto& [_, node] : m_nodes) {
            node.determine_register_nature();
        }*/
        m_immediatePostdominators = create_postdominator_tree();
        find_loops();
    }

    [[nodiscard]] std::optional<node_id> ControlFlowGraph::get_node_with_last_line(const u16 line) const {
        for (const auto& [id, node] : m_nodes) {
            if (node.m_endLine == line) {
                return id;
            }
        }
        return std::nullopt;
    }

    void ControlFlowGraph::insert_node_at_line(const u16 start_line) {
        if (!m_nodes.contains(start_line)) {
            m_nodes.emplace(start_line, start_line);
        }
    }

    void ControlFlowGraph::write_to_txt_file(const std::string& path) const {
        std::ofstream graph_file(path);

        if (!graph_file.is_open()) {
            std::cerr << "couldn't open out graph file " << path << '\n';
        }
        graph_file << "#nodes\n";
        for (const auto& [id, node] : m_nodes) {
            graph_file << id << ' ';
            for (const auto& line : node.m_lines) {
                graph_file << line.m_text << ';';
            }
            graph_file << '\n';
        }

        graph_file << "#edges\n";
        for (const auto& [id, node] : m_nodes) {
            for (const auto& successor : node.m_successors) {
                graph_file << id << ' ' << successor << '\n';
            }
        }
    }
    static std::mutex g_graphviz_mutex;

    void ControlFlowGraph::write_image(const std::string& path) const {
        GVC_t* gvc = gvContext();
        Agraph_t* g = agopen((char*)"G", Agdirected, nullptr);
        std::lock_guard<std::mutex> lock(g_graphviz_mutex);
        
        auto [node_map, max_node] = insert_graphviz_nodes(g);

        insert_graphviz_edges(g, node_map);

        Agraph_t* returng = agsubg(g, const_cast<char*>("return"), 1);
        Agnode_t* return_node = agnode(returng, const_cast<char*>(std::to_string(max_node).c_str()), 1);

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
        }
    }

    [[nodiscard]] std::pair<std::unordered_map<node_id, Agnode_t*>, node_id> ControlFlowGraph::insert_graphviz_nodes(Agraph_t *g) const {
        std::unordered_map<node_id, Agnode_t*> node_map{};
        node_id max_node = 0;
        for (const auto& [node_start, node] : m_nodes) {
            std::string name = std::to_string(node_start);
            Agnode_t* current_node = agnode(g, name.data(), 1);
            node_map[node_start] = current_node;
            if (node_start > max_node) {
                max_node = node_start;
            }
            const std::string node_html_label = node.get_label_html();

            agsafeset_html(current_node, const_cast<char*>("label"), node_html_label.c_str(), "");

            agsafeset(current_node, const_cast<char*>("fontcolor"), accent_color, "");
            agsafeset(current_node, const_cast<char*>("shape"), "plaintext", "");
            agsafeset(current_node, const_cast<char*>("color"), accent_color, "");
        }
        return { node_map, max_node };
    }

    void ControlFlowGraph::insert_graphviz_edges(Agraph_t* g, const std::unordered_map<node_id, Agnode_t*>& node_map) const {
        for (const auto& [node_start, node] : m_nodes) {
            const b8 is_conditional = node.m_lines.back().m_instruction.opcode == Opcode::BranchIf || node.m_lines.back().m_instruction.opcode == Opcode::BranchIfNot;

            for (const auto& next : node.m_successors) {
                Agedge_t* edge = agedge(g, node_map.at(node_start), node_map.at(next), const_cast<char*>(""), 1);
                
                if (node.m_endLine + 1 == next) {
                    agsafeset(edge, const_cast<char*>("color"), is_conditional ? conditional_false_color : fallthrough_color, "");
                }
                else if (next < node_start) {
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
    }

    void ControlFlowGraph::find_loops() {
        for (const auto& loc : m_func->m_stackFrame.m_backwardsJumpLocs) {
            const node_id loop_head = loc.m_target;
            const node_id loop_latch = get_node_with_last_line(loc.m_location).value();
            m_loops.emplace_back(std::move(collect_loop_body(loop_head, loop_latch)), loop_head, loop_latch);
        }
    }

    void dfs(node_id n,
        std::vector<node_id>& rev_postdom,
        std::unordered_map<node_id, u32>& dfsnum,
        std::unordered_set<node_id>& visited,
        const std::map<node_id, control_flow_node>& nodes)
    {
        if (!visited.insert(n).second) {
            return;
        }

        for (auto pred : nodes.at(n).m_predecessors) {
            dfs(pred, rev_postdom, dfsnum, visited, nodes);
        }

        rev_postdom.push_back(n);
    }

    [[nodiscard]] node_id intersect(node_id b1, node_id b2, const std::unordered_map<node_id, node_id>& ipdom, const std::unordered_map<node_id, u32>& dfsnum) {
        while (b1 != b2) {
            while (dfsnum.at(b1) > dfsnum.at(b2)) {
                b1 = ipdom.at(b1);
            }
            while (dfsnum.at(b2) > dfsnum.at(b1)) {
                b2 = ipdom.at(b2);
            }
        }
        return b1;
    }

    [[nodiscard]] std::unordered_map<node_id, node_id>
        ControlFlowGraph::create_postdominator_tree() const
    {
#ifdef _PERF
        const auto start = std::chrono::high_resolution_clock::now();
#endif

        std::vector<node_id> rev_postdom;
        std::unordered_map<node_id, u32> dfsnum;
        rev_postdom.reserve(m_nodes.size());
        dfsnum.reserve(m_nodes.size());
        std::unordered_set<node_id> visited;

        dfs(m_returnNode, rev_postdom, dfsnum, visited, m_nodes);
        std::reverse(rev_postdom.begin(), rev_postdom.end());

        for (u32 i = 0; i < rev_postdom.size(); ++i)
            dfsnum[rev_postdom[i]] = i;

        const u32 N = rev_postdom.size();
        std::unordered_map<node_id, node_id> ipdom;
        for (auto n : rev_postdom)
            ipdom[n] = 0;

        ipdom[m_returnNode] = m_returnNode;

        bool changed = true;
        while (changed) {
            changed = false;
            for (u32 i = 1; i < N; ++i) {
                node_id n = rev_postdom[i];
                node_id new_ipdom = 0;

                for (auto s : m_nodes.at(n).m_successors) {
                    if (ipdom.at(s) != 0) {
                        new_ipdom = s;
                        break;
                    }
                }

                if (new_ipdom == 0)
                    continue;

                for (auto s : m_nodes.at(n).m_successors) {
                    if (ipdom.at(s) != 0 && s != new_ipdom)
                        new_ipdom = intersect(s, new_ipdom, ipdom, dfsnum);
                }

                if (ipdom.at(n) != new_ipdom) {
                    ipdom[n] = new_ipdom;
                    changed = true;
                }
            }
        }

#ifdef _PERF
        const auto end = std::chrono::high_resolution_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << "ms\n";
#endif

        return ipdom;
    }

    [[nodiscard]] std::vector<node_id> ControlFlowGraph::collect_loop_body(const node_id head, const node_id latch) const {
        std::vector<node_id> body;

        const node_id head_ipdom = get_ipdom_at(head);
        const node_id proper_loop_head = head_ipdom != m_nodes.at(latch).m_endLine + 1 ? head_ipdom : head;
        const auto& suc = m_nodes.at(proper_loop_head).get_direct_successor();
        auto it_start = m_nodes.find(head);
        auto it_end = m_nodes.find(latch);
        for (auto& it = it_start; it != it_end; ++it) {
            body.push_back(it->first);
        }
        return body;
    }


    void ControlFlowGraph::get_register_nature(
        const control_flow_node& start_node,
        reg_set regs_to_check,
        reg_set& read_first, 
        const node_id stop_node,
        std::set<node_id>& asd,
        const b8 return_is_read,
        const u32 start_line
    ) const noexcept {

        struct frame {
            const control_flow_node* node;
            reg_set regs;
        };

        std::vector<frame> node_stack;
        std::set<const control_flow_node*> visited;

        node_stack.push_back({&start_node, regs_to_check});

        b8 use_start_line = start_line != 0;

        while (!node_stack.empty()) {
            auto [current_node, regs] = node_stack.back();
            node_stack.pop_back();
            if (visited.contains(current_node)) {
                continue;
            }
            visited.insert(current_node);
            for (u32 i = use_start_line ? start_line : 0; i < current_node->m_lines.size(); ++i) {
                const function_disassembly_line& line = current_node->m_lines[i];
                if (regs.to_ullong() == 0) {
                    break;
                }
                const Instruction& istr = line.m_instruction;
                if (istr.opcode == Opcode::Return && regs.test(istr.destination)) {
                    regs.reset(istr.destination);
                    if (return_is_read) {
                        read_first.set(istr.destination);
                    }
                    break;
                }
                if (istr.destination == istr.operand1 && !istr.operand1_is_immediate() && regs.test(istr.destination)) {
                    regs.reset(istr.destination);
                    read_first.set(istr.destination);
                    continue;
                }
                if (!istr.destination_is_immediate() && istr.destination < ARGUMENT_REGISTERS_IDX && regs.test(istr.destination)) {
                    regs.reset(istr.destination);
                }
                if (istr.op1_is_reg() && istr.operand1 < ARGUMENT_REGISTERS_IDX && regs.test(istr.operand1)) {
                    regs.reset(istr.operand1);
                    read_first.set(istr.operand1);
                }
                if (istr.op2_is_reg() && istr.operand2 < ARGUMENT_REGISTERS_IDX && regs.test(istr.operand2)) {
                    regs.reset(istr.operand2);
                    read_first.set(istr.operand2);
                }
            }
            use_start_line = false;
            if (regs_to_check.to_ullong() == 0) {
                continue;
            }
            for (const auto& successor : current_node->m_successors) {
                if (successor != stop_node) {
                    node_stack.push_back({&m_nodes.at(successor), regs});
                }
            }
        }
    }

    void ControlFlowGraph::get_register_nature(
        const control_flow_node& start_node,
        std::set<reg_idx> regs_to_check,
        std::set<reg_idx>& read_first, 
        const node_id stop_node,
        std::set<node_id>& asd,
        const b8 return_is_read,
        const u32 start_line
    ) const noexcept {

        struct frame {
            const control_flow_node* node;
            std::set<reg_idx> regs;
        };

        std::vector<frame> node_stack;
        std::set<const control_flow_node*> visited;

        node_stack.push_back({&start_node, regs_to_check});

        b8 use_start_line = start_line != 0;

        while (!node_stack.empty()) {
            auto elem = std::move(node_stack.back());
            node_stack.pop_back();
            auto [current_node, regs] = std::move(elem);
            if (visited.contains(current_node)) {
                continue;
            }
            visited.insert(current_node);
            for (u32 i = use_start_line ? start_line : 0; i < current_node->m_lines.size(); ++i) {
                const function_disassembly_line& line = current_node->m_lines[i];
                if (regs.empty()) {
                    break;
                }
                const Instruction& istr = line.m_instruction;
                if (istr.opcode == Opcode::Return && regs.contains(istr.destination)) {
                    regs.erase(istr.destination);
                    if (return_is_read) {
                        read_first.insert(istr.destination);
                    }
                    break;
                }
                if (istr.destination == istr.operand1 && !istr.operand1_is_immediate() && regs.contains(istr.destination)) {
                    regs.erase(istr.destination);
                    read_first.insert(istr.destination);
                    continue;
                }
                if (regs.contains(istr.destination) && !istr.destination_is_immediate()) {
                    regs.erase(istr.destination);
                }
                if (regs.contains(istr.operand1) && istr.op1_is_reg()) {
                    regs.erase(istr.operand1);
                    read_first.insert(istr.operand1);
                }
                if (regs.contains(istr.operand2) && istr.op2_is_reg()) {
                    regs.erase(istr.operand2);
                    read_first.insert(istr.operand2);
                }
            }
            use_start_line = false;
            if (regs_to_check.empty()) {
                continue;
            }
            for (const auto& successor : current_node->m_successors) {
                if (successor != stop_node) {
                    node_stack.push_back({&m_nodes.at(successor), regs});
                }
            }
        }
    }

    u16 ControlFlowGraph::get_register_read_count(
        const control_flow_node& start_node,
        const reg_idx reg_to_check,
        const node_id stop_node,
        std::set<node_id>& checked,
        const b8 return_is_read,
        const u32 start_line
    ) const noexcept {
        if (checked.contains(start_node.m_startLine)) {
            return 0;
        }
        checked.insert(start_node.m_startLine);
        u16 count = 0;
        for (u32 i = start_line; i < start_node.m_lines.size(); ++i) {
            if (count >= 2) {
                return count;
            }
            const function_disassembly_line& line = start_node.m_lines[i];
            const Instruction& istr = line.m_instruction;
            if (reg_to_check == istr.destination && istr.opcode == Opcode::Return) {
                if (return_is_read) {
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
        if (start_node.m_startLine == stop_node) {
            return count;
        }
        for (const auto& successor : start_node.m_successors) {
            if (count >= 2) {
                break;
            }
            count += get_register_read_count(m_nodes.at(successor), reg_to_check, stop_node, checked, return_is_read);
        }
        return count;
    }

    void ControlFlowGraph::get_registers_written_to(const control_flow_node& node, const node_id stop, reg_set& result, std::set<node_id>& checked) const {
        if (node.m_startLine == stop || checked.contains(node.m_startLine)) {
            return;
        }
        checked.insert(node.m_startLine);
        for (const auto& line : node.m_lines) {
            if (!line.m_instruction.destination_is_immediate() && line.m_instruction.destination < ARGUMENT_REGISTERS_IDX) {
                result.set(line.m_instruction.destination);
            }
        }
        for (const auto& successor : node.m_successors) {
            get_registers_written_to(m_nodes.at(successor), stop, result, checked);
        }
    }
    
    [[nodiscard]] reg_set ControlFlowGraph::get_branch_phi_registers(const control_flow_node& start_node, const b8 return_is_read) const noexcept {
        reg_set regs_to_check, read_first_branches, read_first_ipdom, left, right, result;
        std::set<node_id> checked;
        node_id ipdom = get_ipdom_at(start_node.m_startLine);

        const node_id target = start_node.get_adjusted_target();

        if (ipdom == start_node.m_startLine) {
            result.set(target);
            return result;
        }

        get_registers_written_to(m_nodes.at(start_node.get_direct_successor()), ipdom, left, checked);
        checked.clear();
        get_registers_written_to(m_nodes.at(target), ipdom, right, checked);
        checked.clear();

        regs_to_check = left | right;
        get_register_nature(m_nodes.at(ipdom), regs_to_check, read_first_ipdom, -1, checked, return_is_read);

        return read_first_ipdom;
    }

    [[nodiscard]] reg_set ControlFlowGraph::get_loop_phi_registers(const control_flow_node& head_node) const noexcept {
        reg_set regs_to_check, read_first, result;
        std::set<node_id> checked;
        for (const auto& line : head_node.m_lines) {
            if (!line.m_instruction.destination_is_immediate() && line.m_instruction.operand1 < ARGUMENT_REGISTERS_IDX) {
                regs_to_check.set(line.m_instruction.destination);
            }
        }
        get_registers_written_to(m_nodes.at(head_node.get_direct_successor()), head_node.m_startLine, regs_to_check, checked);
        checked.clear();
        get_register_nature(m_nodes.at(head_node.get_adjusted_target()), regs_to_check, read_first, m_returnNode, checked, true);

        return read_first;
    }

    [[nodiscard]] const control_flow_node& ControlFlowGraph::get_final_loop_condition_node(const control_flow_loop& loop, const node_id exit_node) const noexcept {
        auto it_start = m_nodes.find(loop.m_headNode);
        auto it_end = m_nodes.find(loop.m_latchNode);
        for (auto& it = it_end; it != it_start; --it) {
            if (it->second.get_adjusted_target() == exit_node) {
                return it->second;
            }
        }
        return m_nodes.at(loop.m_headNode);
    }
}