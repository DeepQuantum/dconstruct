#pragma once

#include "base.h"
#include "instructions.h"
#include <vector>

namespace dconstruct {

    struct ControlFlowNode {
        std::vector<FunctionDisassemblyLine> m_lines{};
        std::vector<const ControlFlowNode*> m_successors{};
        u32 m_startLine;
        u32 m_endLine;

        explicit ControlFlowNode(const u32 line) noexcept {
            m_startLine = line;
            m_endLine = 0;
        }

        [[nodiscard]] std::string get_label_html() const noexcept;
    };

    struct ControlFlowLoop {
        std::vector<u32> m_body;
        ControlFlowNode* m_headNode;
        ControlFlowNode* m_latchNode;
    };

    class ControlFlowGraph {
    public:
        explicit ControlFlowGraph() = delete;
        explicit ControlFlowGraph(const FunctionDisassembly *) noexcept;
        [[nodiscard]] std::vector<ControlFlowLoop> find_loops() noexcept;
        void write_to_txt_file(const std::string& path = "graph.txt") const noexcept;
        void write_image(const std::string& path = "graph.svg") const noexcept;
    private:
        std::vector<std::unique_ptr<ControlFlowNode>> m_nodes;
        std::map<ControlFlowNode*, std::vector<ControlFlowNode*>> m_predecessors{};
        std::map<u32, u32> m_immediateDominators{};
        const FunctionDisassembly *m_func;

        ControlFlowNode* insert_node_at_line(const u32 start_line) noexcept;
        const ControlFlowNode* get_node_at_line(const u32 start_line) const noexcept;
        const ControlFlowNode* get_node_containing_line(const u32 line) const noexcept;
        
        [[nodiscard]] std::map<const ControlFlowNode*, std::vector<const ControlFlowNode*>> compute_predecessors() const noexcept;
        [[nodiscard]] b8 dominates(const ControlFlowNode *n, const ControlFlowNode *m) const noexcept;
        void find_immediate_dominators() noexcept;
        [[nodiscard]] std::vector<u32> get_idom_predecessors(const u32 n) noexcept;
        [[nodiscard]] u32 intersect(const u32 n1, const u32 n2) const noexcept;
    };

    
}