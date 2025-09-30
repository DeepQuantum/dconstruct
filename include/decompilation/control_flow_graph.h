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

namespace dconstruct {

    using node_id = u32;

    struct control_flow_node {
        std::vector<function_disassembly_line> m_lines{};
        std::vector<node_id> m_successors{};
        u32 m_startLine = 0;
        u32 m_endLine = 0;

        explicit control_flow_node() = default;

        explicit control_flow_node(const u32 line) : m_startLine(line) {}

        [[nodiscard]] b8 operator==(const control_flow_node& rhs) const noexcept;
        [[nodiscard]] b8 operator!=(const control_flow_node& rhs) const noexcept;

        [[nodiscard]] std::string get_label_html() const;

        [[nodiscard]] node_id get_direct_successor() const;
        
        [[nodiscard]] const function_disassembly_line& get_last_line() const;
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
        ControlFlowGraph(const function_disassembly* func, std::unordered_map<node_id, control_flow_node> nodes) : ControlFlowGraph(func) {
            m_nodes = std::move(nodes);
        };
        void find_loops();
        void write_to_txt_file(const std::string& path = "graph.txt") const;
        void write_image(const std::string &path = "graph.svg") const;

        void insert_loop_subgraphs(Agraph_t *g) const;

        [[nodiscard]] const std::unordered_map<node_id, control_flow_node>& get_nodes() const {
            return m_nodes;
        }

        [[nodiscard]] const control_flow_node& operator[](const u32 at) const {
            return m_nodes.at(at);
        }

        [[nodiscard]] const std::unordered_map<node_id, node_id>& get_immediate_postdominators() const {
            return m_immediatePostdominators;
        }

        [[nodiscard]] opt_ref<const control_flow_loop> get_loop_with_head(const node_id node) const;

        [[nodiscard]] const control_flow_node& get_return_node() const noexcept {
            return m_nodes.at(m_returnNode);
        }

        [[nodiscard]] b8 register_gets_read_before_overwrite(const node_id node_id, const u32 check_register, const u32 start_line = 0) const noexcept;

        // get the registers in a node that will be read from by the successors, so we know which registers we need to emit
        [[nodiscard]] std::set<u32> get_variant_registers(const node_id node) const noexcept;
        void get_variant_registers_recursive(const node_id node, std::set<u32>& read_after_regs, std::set<u32>& written_to_regs, std::set<u32>& written_firstm, u32 start_line = 0) const noexcept;
        

    private:
        std::unordered_map<node_id, control_flow_node> m_nodes;
        std::unordered_map<node_id, node_id> m_immediatePostdominators;
        std::vector<control_flow_loop> m_loops;
        node_id m_returnNode;
        const function_disassembly *m_func;

        void insert_node_at_line(const node_id start_line);
        [[nodiscard]] std::optional<node_id> get_node_with_last_line(const u32 last_line) const;

        [[nodiscard]] std::pair<std::unordered_map<node_id, Agnode_t*>, node_id> insert_graphviz_nodes(Agraph_t* g) const;
        void insert_graphviz_edges(Agraph_t* g, const std::unordered_map<node_id, Agnode_t*>& node_map) const;

        [[nodiscard]] std::unordered_map<node_id, node_id> create_postdominator_tree() const;


        [[nodiscard]] b8 dominates(const node_id, const node_id) const;
        [[nodiscard]] b8 dominee_not_found_outside_dominator_path(node_id current_head, const node_id dominator, const node_id dominee, std::unordered_set<node_id>& visited) const;
        [[nodiscard]] std::vector<node_id> collect_loop_body(const node_id, const node_id) const;
        
        //[[nodiscard]] std::map<node_id, std::vector<node_id>> compute_predecessors() const;

        void add_successors(std::vector<node_id>& nodes, const control_flow_node& node, const control_flow_node& stop) const;
    };

    
}