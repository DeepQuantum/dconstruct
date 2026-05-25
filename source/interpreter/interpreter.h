#include "base.h"
#include "disassembly/instructions.h"


namespace dconstruct::interpreter {
    
    struct interpreter {
        

        std::array<u64, MAX_REGISTER> m_stackFrame;
        
        
        u64 run(const std::vector<Instruction>& instructions, const std::vector<u64>& symbol_table) noexcept;
        
    };
    
    
}
