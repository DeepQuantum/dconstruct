#pragma once

#include "base.h"
#include "disassembly/instructions.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>
#include <optional>

namespace dconstruct {

    using node_id = u32;

    struct control_flow_node {
        std::vector<function_disassembly_line> m_lines{};
        std::vector<node_id> m_successors{};
        u32 m_startLine = 0;
        u32 m_endLine = 0;

        explicit control_flow_node() = default;

        explicit control_flow_node(const u32 line) : m_startLine(line) {}

        [[nodiscard]] std::string get_label_html() const;

        [[nodiscard]] node_id get_direct_successor() const;
    };

    struct control_flow_loop {
        std::vector<node_id> m_body;
        node_id m_headNode;
        node_id m_latchNode;
    };

    class ControlFlowGraph {
    public:
        ControlFlowGraph() = delete;
        explicit ControlFlowGraph(const function_disassembly *);
        void find_loops();
        void write_to_txt_file(const std::string& path = "graph.txt") const;
        void write_image(const std::string &path = "graph.svg") const;

        void insert_loop_subgraphs(Agraph_t *g) const;

        [[nodiscard]] const control_flow_node& operator[](const u32 at) const {
            return m_nodes.at(at);
        }

        [[nodiscard]] std::optional<std::reference_wrapper<const control_flow_loop>> get_loop_with_head(const node_id node) const;

    private:
        std::map<node_id, control_flow_node> m_nodes;
        std::map<node_id, node_id> m_immediatePostdominators;
        std::vector<control_flow_loop> m_loops;
        const function_disassembly *m_func;

        [[nodiscard]] void insert_node_at_line(const node_id start_line);
        [[nodiscard]] std::optional<node_id> get_node_with_last_line(const u32 last_line) const;

        [[nodiscard]] std::pair<std::map<node_id, Agnode_t*>, node_id> insert_graphviz_nodes(Agraph_t* g) const;
        void insert_graphviz_edges(Agraph_t* g, const std::map<node_id, Agnode_t*>& node_map) const;


        [[nodiscard]] b8 dominates(const node_id, const node_id) const;
        [[nodiscard]] b8 dominee_not_found_outside_dominator_path(node_id current_head, const node_id dominator, const node_id dominee, std::unordered_set<node_id>& visited) const;
        [[nodiscard]] std::vector<node_id> collect_loop_body(const node_id, const node_id) const;
        
        [[nodiscard]] std::map<node_id, node_id> create_postdominator_tree() const;
        //[[nodiscard]] std::map<node_id, std::vector<node_id>> compute_predecessors() const;

        void add_successors(std::vector<node_id>& nodes, const control_flow_node& node, const control_flow_node& stop) const;
    };

    
}