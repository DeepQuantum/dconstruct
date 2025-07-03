#include "base.h"
#include "instructions.h"
#include <vector>

namespace dconstruct {

    struct ControlFlowNode {
        u32 m_startLine;
        u32 m_endLine;
        std::vector<u32> m_incoming{};
        std::vector<u32> m_outgoing{};
        std::vector<FunctionDisassemblyLine> m_lines{};


    };
}