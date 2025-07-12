#pragma once
#define GVDLL

#include "base.h"
#include "instructions.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>

namespace dconstruct {

    struct ControlFlowNode {
        std::vector<FunctionDisassemblyLine> m_lines{};
        std::vector<const ControlFlowNode*> m_successors{};
        u32 m_startLine = 0;
        u32 m_endLine = 0;

        explicit ControlFlowNode() noexcept = default;

        explicit ControlFlowNode(const u32 line) noexcept : m_startLine(line) {}

        [[nodiscard]] std::string get_label_html() const noexcept;
    };

    struct ControlFlowLoop {
        const std::vector<const ControlFlowNode*> m_body;
        const ControlFlowNode* m_headNode;
        const ControlFlowNode* m_latchNode;
    };

    class ControlFlowGraph {
    public:
        explicit ControlFlowGraph() = delete;
        explicit ControlFlowGraph(const FunctionDisassembly *) noexcept;
        void find_loops() noexcept;
        void write_to_txt_file(const std::string& path = "graph.txt") const noexcept;
        void write_image(const std::string &path = "graph.svg") const noexcept;

void insert_loop_subgraphs(Agraph_t * g) const;

    private:
        std::unordered_map<u32, ControlFlowNode> m_nodes{};
        std::vector<ControlFlowLoop> m_loops{};
        std::map<ControlFlowNode*, std::vector<ControlFlowNode*>> m_predecessors{};
        std::map<u32, u32> m_immediateDominators{};
        const FunctionDisassembly *m_func;

        [[nodiscard]] ControlFlowNode* insert_node_at_line(const u32 start_line) noexcept;
        [[nodiscard]] const ControlFlowNode* get_node_with_last_line(const u32 line) const noexcept;

        [[nodiscard]] std::pair<std::unordered_map<u32, Agnode_t*>, u32> insert_graphviz_nodes(Agraph_t* g) const noexcept;
        void insert_graphviz_edges(Agraph_t* g, const std::unordered_map<u32, Agnode_t*>& node_map) const noexcept;

        [[nodiscard]] b8 dominates(const ControlFlowNode*, const ControlFlowNode*) const noexcept;
        [[nodiscard]] static b8 dominee_not_found_outside_dominator_path(const ControlFlowNode* current_head, const ControlFlowNode* dominator, const ControlFlowNode* dominee, std::unordered_set<const ControlFlowNode*>& visited) noexcept;
        [[nodiscard]] std::vector<const ControlFlowNode*> collect_loop_body(const ControlFlowNode*, const ControlFlowNode*) const noexcept;
        
        [[nodiscard]] std::map<const ControlFlowNode*, std::vector<const ControlFlowNode*>> compute_predecessors() const noexcept;
        /*void find_immediate_dominators() noexcept;
        [[nodiscard]] std::vector<u32> get_idom_predecessors(const u32 n) noexcept;
        [[nodiscard]] u32 intersect(const u32 n1, const u32 n2) const noexcept;*/
    };

    
}