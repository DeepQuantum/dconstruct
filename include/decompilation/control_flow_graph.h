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

    struct register_nature {
        reg_set m_readFirst;
        reg_set m_readTwice;
        reg_set m_written;
    };

    struct control_flow_node {
        std::vector<function_disassembly_line> m_lines;
        std::vector<node_id> m_predecessors;
        register_nature m_regs;
        node_id m_followingNode = 0;
        node_id m_targetNode = 0;
        istr_line m_startLine = 0;
        istr_line m_endLine = 0;
        node_id m_index = 0;
        node_id m_postorder = 0;
        node_id m_ipdom = 0;

        explicit control_flow_node() = default;

        explicit control_flow_node(const istr_line line) : m_startLine(line) {}

        [[nodiscard]] bool operator==(const control_flow_node& rhs) const noexcept;
        [[nodiscard]] bool operator!=(const control_flow_node& rhs) const noexcept;

        [[nodiscard]] std::string get_label_html() const;
        void determine_register_nature();
        [[nodiscard]] register_nature get_register_nature_starting_at(const istr_line start_line, const bool return_is_read) const noexcept;
    };

    struct control_flow_loop {
        std::vector<node_id> m_body;
        node_id m_headNode;
        node_id m_latchNode;
    };

    

    [[nodiscard]] const control_flow_node& intersect(const node_id node_b1, const node_id node_b2, const std::vector<control_flow_node>& nodes);

    class ControlFlowGraph {
    public:
        std::vector<control_flow_node> m_nodes;
        std::vector<control_flow_loop> m_loops;

        const function_disassembly &m_func;

        ControlFlowGraph() = default;
        explicit ControlFlowGraph(const function_disassembly& fn);
        explicit ControlFlowGraph(const function_disassembly& func, std::vector<control_flow_node> nodes) : ControlFlowGraph(func) {
            m_nodes = std::move(nodes);
        };
        void find_loops();
        void write_to_txt_file(const std::string& path = "graph.txt") const;
        void write_image(const std::string &path = "graph.svg") const;
        
        void insert_loop_subgraphs(Agraph_t *g) const;

        [[nodiscard]] inline const control_flow_node& operator[](const node_id at) const {
            return m_nodes[at];
        }

        [[nodiscard]] const control_flow_loop* get_loop_with_head(const node_id node) const;

        [[nodiscard]] reg_set get_registers_written_to(const control_flow_node& node, const node_id stop) const;
        [[nodiscard]] reg_set get_branch_phi_registers(const control_flow_node& start_node) const noexcept;
        [[nodiscard]] reg_set get_loop_phi_registers(const control_flow_node& head_node) const noexcept;

        [[nodiscard]] reg_set get_register_nature(const control_flow_node& start_node, reg_set check_regs, const node_id stop_node, istr_line start_line = 0) const noexcept;
        
        u8 get_register_read_count(const control_flow_node& start_node, const reg_idx reg_to_check, const istr_line start_line = 0) const noexcept;
        [[nodiscard]] const control_flow_node& get_final_loop_condition_node(const control_flow_loop& loop, const node_id exit_node) const noexcept;
    
        private:

        void compute_postdominators();

        [[nodiscard]] std::vector<Agnode_t*> insert_graphviz_nodes(Agraph_t* g) const;
        void insert_graphviz_edges(Agraph_t* g, const std::vector<Agnode_t*>& nodes) const;

        [[nodiscard]] std::vector<node_id> collect_loop_body(const node_id, const node_id) const;
    };

    
}