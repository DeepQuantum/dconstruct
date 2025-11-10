#pragma once

#include "base.h"
#include "disassembly/instructions.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>
#include <optional>
#include <set>
#include <bitset>

namespace dconstruct {

    using node_id = u32;

    struct control_flow_node {
        std::vector<function_disassembly_line> m_lines;
        std::vector<node_id> m_successors;
        std::vector<node_id> m_predecessors;
        reg_set m_readFirstRegs;
        reg_set m_writtenToRegs;
        u32 m_startLine = 0;
        u32 m_endLine = 0;

        explicit control_flow_node() = default;

        explicit control_flow_node(const u32 line) : m_startLine(line) {}

        [[nodiscard]] b8 operator==(const control_flow_node& rhs) const noexcept;
        [[nodiscard]] b8 operator!=(const control_flow_node& rhs) const noexcept;

        [[nodiscard]] std::string get_label_html() const;

        [[nodiscard]] node_id get_direct_successor() const;
        
        [[nodiscard]] const function_disassembly_line& get_last_line() const;

        [[nodiscard]] node_id get_adjusted_target() const;
        void determine_register_nature();
    };

    struct control_flow_loop {
        std::vector<node_id> m_body;
        node_id m_headNode;
        node_id m_latchNode;
    };

    class ControlFlowGraph {
    public:
        ControlFlowGraph() = default;
        explicit ControlFlowGraph(const function_disassembly *);
        ControlFlowGraph(const function_disassembly* func, std::map<node_id, control_flow_node> nodes) : ControlFlowGraph(func) {
            m_nodes = std::move(nodes);
        };
        void find_loops();
        void write_to_txt_file(const std::string& path = "graph.txt") const;
        void write_image(const std::string &path = "graph.svg") const;
        
        void insert_loop_subgraphs(Agraph_t *g) const;
        std::set<node_id> m_ipdomsEmitted;


        [[nodiscard]] const std::map<node_id, control_flow_node>& get_nodes() const {
            return m_nodes;
        }

        [[nodiscard]] const control_flow_node& operator[](const u32 at) const {
            return m_nodes.at(at);
        }

        [[nodiscard]] inline node_id get_ipdom_at(const node_id node) const {
            return m_immediatePostdominators.at(node);
        }

        [[nodiscard]] opt_ref<const control_flow_loop> get_loop_with_head(const node_id node) const;

        [[nodiscard]] const control_flow_node& get_return_node() const noexcept {
            return m_nodes.at(m_returnNode);
        }

        void get_registers_written_to(const control_flow_node& node, const node_id stop, std::set<reg_idx>& result, std::set<node_id>& checked) const;

        [[nodiscard]] std::set<reg_idx> get_branch_phi_registers(const control_flow_node& start_node, const b8 return_is_read) const noexcept;
        [[nodiscard]] std::set<reg_idx> get_loop_phi_registers(const control_flow_node& head_node) const noexcept;


        void get_register_nature(const control_flow_node& start_node, std::set<reg_idx> check_regs, std::set<reg_idx>& read_first, const node_id stop_node, std::set<node_id>& checked, const b8 return_is_read, const u32 start_line = 0) const noexcept;
        u16 get_register_read_count(const control_flow_node& start_node, const reg_idx reg_to_check, const node_id stop_node, std::set<node_id>& checked, const b8 return_is_read, const u32 start_line = 0) const noexcept;
        [[nodiscard]] const control_flow_node& get_final_loop_condition_node(const control_flow_loop& loop, const node_id exit_node) const noexcept;
    private:
        std::map<node_id, control_flow_node> m_nodes;
        std::unordered_map<node_id, node_id> m_immediatePostdominators;
        std::vector<control_flow_loop> m_loops;
        node_id m_returnNode;
        const function_disassembly *m_func;
        [[nodiscard]] std::unordered_map<node_id, node_id> create_postdominator_tree() const;


        void insert_node_at_line(const node_id start_line);
        [[nodiscard]] std::optional<node_id> get_node_with_last_line(const u32 last_line) const;

        [[nodiscard]] std::pair<std::unordered_map<node_id, Agnode_t*>, node_id> insert_graphviz_nodes(Agraph_t* g) const;
        void insert_graphviz_edges(Agraph_t* g, const std::unordered_map<node_id, Agnode_t*>& node_map) const;

        


        [[nodiscard]] std::vector<node_id> collect_loop_body(const node_id, const node_id) const;
        
        //[[nodiscard]] std::map<node_id, std::vector<node_id>> compute_predecessors() const;



    };

    
}