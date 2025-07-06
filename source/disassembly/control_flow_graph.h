#include "base.h"
#include "instructions.h"
#include <vector>

namespace dconstruct {

    struct ControlFlowNode {
        u32 m_startLine;
        u32 m_endLine;
        std::vector<ControlFlowNode*> m_next{};
        std::vector<FunctionDisassemblyLine> m_lines{};

        explicit ControlFlowNode(const u32 line) noexcept {
            m_startLine = line;
        }
    };
}