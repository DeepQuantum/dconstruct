#pragma once

#include "base.h"
#include "disassembly/instructions.h"

namespace dconstruct::compiler {
    struct dc_code_generator {

        std::vector<std::byte> m_bytestream;

        void emit_instruction(const Instruction& istr) noexcept;
        void emit_instruction(const Opcode opcode, const u8 dest, const u8 op1, const u8 op2) noexcept;

    };
}