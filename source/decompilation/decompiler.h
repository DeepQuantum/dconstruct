#include "base.h"
#include "instructions.h"
#include "control_flow_graph.h"
#include <sstream>

namespace dconstruct {

struct DecompiledFunction {};

class Decompiler { 

public:

    explicit Decompiler() = delete;

    explicit Decompiler(const FunctionDisassembly *func) {
        m_functions.push_back(func);
    };

    explicit Decompiler(const std::vector<const FunctionDisassembly*> &funcs) {
        m_functions = funcs;
    };

    std::vector<DecompiledFunction> decompile() noexcept;

private:
    std::vector<const FunctionDisassembly*> m_functions{};
};

}