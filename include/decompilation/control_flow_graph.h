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

    struct control_flow_node {
        std::vector<function_disassembly_line> m_lines{};
        std::vector<const control_flow_node*> m_successors{};
        u32 m_startLine = 0;
        u32 m_endLine = 0;

        explicit control_flow_node() = default;

        explicit control_flow_node(const u32 line) : m_startLine(line) {}

        [[nodiscard]] std::string get_label_html() const;

        [[nodiscard]] const control_flow_node* get_direct_successor() const;
    };

    struct control_flow_loop {
        const std::vector<const control_flow_node*> m_body;
        const control_flow_node* m_headNode;
        const control_flow_node* m_latchNode;
    };

    class ControlFlowGraph {
    public:
        ControlFlowGraph() = delete;
        explicit ControlFlowGraph(const function_disassembly *);
        void find_loops();
        void write_to_txt_file(const std::string& path = "graph.txt") const;
        void write_image(const std::string &path = "graph.svg") const;

        void insert_loop_subgraphs(Agraph_t *g) const;

        const std::map<u32, control_flow_node>& get_nodes() const noexcept {
            return m_nodes;
        }

        [[nodiscard]] std::optional<std::reference_wrapper<const control_flow_loop>> get_loop_with_head(const control_flow_node& node) const;

    private:
        std::map<u32, control_flow_node> m_nodes{};
        std::vector<control_flow_loop> m_loops{};
        std::map<control_flow_node*, std::vector<control_flow_node*>> m_predecessors{};
        std::map<u32, u32> m_immediateDominators{};
        const function_disassembly *m_func;

        [[nodiscard]] control_flow_node* insert_node_at_line(const u32 start_line);
        [[nodiscard]] const control_flow_node* get_node_with_last_line(const u32 line) const;

        [[nodiscard]] std::pair<std::map<u32, Agnode_t*>, u32> insert_graphviz_nodes(Agraph_t* g) const;
        void insert_graphviz_edges(Agraph_t* g, const std::map<u32, Agnode_t*>& node_map) const;

        [[nodiscard]] const control_flow_node* get_immediate_postdominator(const control_flow_node*);

        [[nodiscard]] b8 dominates(const control_flow_node*, const control_flow_node*) const;
        [[nodiscard]] static b8 dominee_not_found_outside_dominator_path(const control_flow_node* current_head, const control_flow_node* dominator, const control_flow_node* dominee, std::unordered_set<const control_flow_node*>& visited);
        [[nodiscard]] std::vector<const control_flow_node*> collect_loop_body(const control_flow_node*, const control_flow_node*) const;
        
        [[nodiscard]] std::map<const control_flow_node*, std::vector<const control_flow_node*>> compute_predecessors() const;
        /*void find_immediate_dominators() noexcept;
        [[nodiscard]] std::vector<u32> get_idom_predecessors(const u32 n) noexcept;
        [[nodiscard]] u32 intersect(const u32 n1, const u32 n2) const noexcept;*/
    };

    
}