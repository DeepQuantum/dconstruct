#include "base.h"
#include "instructions.h"
#include "control_flow_graph.h"

namespace dconstruct {

struct DecompiledFunction {



};

class Decompiler { 

public:

    explicit Decompiler() = delete;
    explicit Decompiler(FunctionDisassembly *func): m_functionDisassembly(func) {};

    DecompiledFunction decompile() noexcept;

private:
    FunctionDisassembly *m_functionDisassembly = nullptr;
    std::map<u32, std::unique_ptr<ControlFlowNode>> m_nodes{};
    std::map<u32, std::vector<u32>> m_predecessors{};
    std::map<u32, u32> m_immediateDominators{};
    ControlFlowNode *m_controlFlowHead = nullptr;

    void parse_control_flow_graph() noexcept;
    void write_control_flow_graph_txt_file(const std::string &path = "graph.txt") const noexcept;
    void write_control_flow_graph_image(const std::string &path = "graph.svg") const noexcept;
    [[nodiscard]] std::string create_node_text(const ControlFlowNode *) const noexcept;
    void compute_predecessors() noexcept;
    void find_immediate_dominators() noexcept;
    [[nodiscard]] std::vector<u32> get_idom_predecessors(const u32 n) const noexcept;
};

}