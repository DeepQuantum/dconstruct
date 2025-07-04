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
    void output_cfg_file(const std::string &path = "graph.txt") const noexcept;


private:
    FunctionDisassembly *m_functionDisassembly = nullptr;
    std::map<u32, std::unique_ptr<ControlFlowNode>> m_nodes;
    ControlFlowNode *m_controlFlowHead;
};

}