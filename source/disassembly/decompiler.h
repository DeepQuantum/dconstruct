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
    void parse_control_flow_graph() noexcept;
    void insert_new_node(u32, const ControlFlowNode &) noexcept;
    void write_control_flow_graph_txt_file(const std::string &path = "graph.txt") const noexcept;
    void write_control_flow_graph_image(const std::string &path = "graph.svg") const noexcept;
    [[nodiscard]] std::string create_node_text(const ControlFlowNode *node) const noexcept;


private:
    FunctionDisassembly *m_functionDisassembly = nullptr;
    std::map<u32, std::unique_ptr<ControlFlowNode>> m_nodes;
    ControlFlowNode *m_controlFlowHead = nullptr;
};

}