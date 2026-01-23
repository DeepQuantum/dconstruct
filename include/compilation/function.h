#pragma once

#include "base.h"
#include "disassembly/instructions.h"
#include "environment.h"
#include <unordered_map>

namespace dconstruct::ast {
    struct expression;
}

namespace dconstruct::compiler {
    struct function {
        void emit_instruction(const Opcode opcode, const u8 destination, const u8 operand1 = 0, const u8 operand2 = 0) noexcept;

        [[nodiscard]] std::optional<reg_idx> get_next_unused_register() noexcept;
        void free_register(const ast::expression& expr, const reg_idx reg) noexcept;
        
        [[nodiscard]] u8 add_to_symbol_table(const u64 value) noexcept;

        [[nodiscard]] u64 get_size_in_bytes() const noexcept;

        std::vector<Instruction> m_instructions;
        std::vector<u64> m_symbolTable;
        environment<reg_idx> m_varsToRegs;
        std::string m_name;
        argument_reg_set m_argumentUsedRegisters = 0;
        reg_set m_usedRegisters = 0;
    };
}