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

    struct control_flow_node {
        std::vector<function_disassembly_line> m_lines;
        std::vector<node_id> m_predecessors;
        node_id m_directSuccessor = 0;
        node_id m_targetSuccessor = 0;
        reg_set m_readFirstRegs = 0;
        reg_set m_writtenToRegs = 0;
        node_id m_startLine = 0;
        node_id m_endLine = 0;
        node_id m_index = 0;
        node_id m_revPostorder = 0;
        node_id m_ipdom = 0;

        explicit control_flow_node() = default;

        explicit control_flow_node(const node_id line) : m_startLine(line) {}

        [[nodiscard]] b8 operator==(const control_flow_node& rhs) const noexcept;
        [[nodiscard]] b8 operator!=(const control_flow_node& rhs) const noexcept;

        [[nodiscard]] std::string get_label_html() const;
        void determine_register_nature();
    };

    struct control_flow_loop {
        std::vector<node_id> m_body;
        node_id m_headNode;
        node_id m_latchNode;
    };

    class ControlFlowGraph {
    public:
        std::vector<control_flow_node> m_nodes;
        //std::unordered_map<node_id, node_id> m_immediatePostdominators;
        std::vector<control_flow_loop> m_loops;
        //node_id m_returnNode;
        const function_disassembly *m_func;
        ControlFlowGraph() = default;
        explicit ControlFlowGraph(const function_disassembly *);
        ControlFlowGraph(const function_disassembly* func, std::vector<control_flow_node> nodes) : ControlFlowGraph(func) {
            m_nodes = std::move(nodes);
        };
        void find_loops();
        void write_to_txt_file(const std::string& path = "graph.txt") const;
        void write_image(const std::string &path = "graph.svg") const;
        
        void insert_loop_subgraphs(Agraph_t *g) const;
        std::set<node_id> m_ipdomsEmitted;

        [[nodiscard]] const std::vector<control_flow_node>& get_nodes() const {
            return m_nodes;
        }

        [[nodiscard]] const control_flow_node& operator[](const node_id at) const {
            return m_nodes[at];
        }

        [[nodiscard]] opt_ref<const control_flow_loop> get_loop_with_head(const node_id node) const;

        void get_registers_written_to(const control_flow_node& node, const node_id stop, reg_set& result, node_set& checked) const;
        [[nodiscard]] reg_set get_branch_phi_registers(const control_flow_node& start_node, const b8 return_is_read) const noexcept;
        [[nodiscard]] reg_set get_loop_phi_registers(const control_flow_node& head_node) const noexcept;

        void get_register_nature(const control_flow_node& start_node, reg_set check_regs, reg_set& read_first, const node_id stop_node, node_set& checked, const b8 return_is_read, const u32 start_line = 0) const noexcept;
        
        u16 get_register_read_count(const control_flow_node& start_node, const reg_idx reg_to_check, const node_id stop_node, node_set& checked, const b8 return_is_read, const u32 start_line = 0) const noexcept;
        [[nodiscard]] const control_flow_node& get_final_loop_condition_node(const control_flow_loop& loop, const node_id exit_node) const noexcept;
    
        private:
        
        void compute_postdominators();


        [[nodiscard]] opt_ref<const control_flow_node> get_node_with_last_line(const node_id last_line) const;

        [[nodiscard]] std::vector<Agnode_t*> insert_graphviz_nodes(Agraph_t* g) const;
        void insert_graphviz_edges(Agraph_t* g, const std::vector<Agnode_t*>& nodes) const;

        [[nodiscard]] std::vector<node_id> collect_loop_body(const node_id, const node_id) const;
    };

    
}