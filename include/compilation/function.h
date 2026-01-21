#pragma once

#include "base.h"
#include "disassembly/instructions.h"


namespace dconstruct::compiler {
    struct function {
        std::vector<Instruction> m_instructions;
        std::vector<u64> m_symbolTable;

        void emit_instruction(const Opcode opcode, const u8 destination, const u8 operand1 = 0, const u8 operand2 = 0) noexcept;

        [[nodiscard]] std::optional<reg_idx> get_next_unused_register() noexcept;
        void free_register(const reg_idx reg) noexcept;
        
        [[nodiscard]] u8 add_to_symbol_table(const u64 value) noexcept;

        reg_set m_usedRegisters = 0;
        argument_reg_set m_argumentUsedRegisters = 0;
    };
}