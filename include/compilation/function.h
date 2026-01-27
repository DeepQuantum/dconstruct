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

        enum class SYMBOL_TABLE_POINTER_KIND {
            NONE,
            STRING,
            GENERAL,
        };

        void emit_instruction(const Opcode opcode, const u8 destination, const u8 operand1 = 0, const u8 operand2 = 0) noexcept;

        [[nodiscard]] std::expected<reg_idx, std::string> get_next_unused_register() noexcept;
        void free_register(const reg_idx reg) noexcept;
        void free_register(const ast::expression& expr, const reg_idx reg) noexcept;
        std::optional<std::string> save_used_argument_registers(const u8 count) noexcept;
        void restore_used_argument_registers() noexcept;
        
        [[nodiscard]] u8 add_to_symbol_table(const u64 value, const SYMBOL_TABLE_POINTER_KIND pointer_kind = SYMBOL_TABLE_POINTER_KIND::NONE) noexcept;

        [[nodiscard]] u64 get_size_in_bytes() const noexcept;
        [[nodiscard]] u64 get_scriptlambda_sum() const noexcept;

        std::vector<Instruction> m_instructions;
        std::vector<u64> m_symbolTable;
        std::vector<SYMBOL_TABLE_POINTER_KIND> m_symbolTableEntryPointers;
        std::vector<u8> m_numberSavedArgumentsStack;
        std::vector<reg_set> m_savedArgumentsTemporaryRegs;
        environment<reg_idx> m_varsToRegs;
        std::variant<std::string, sid64> m_name;
        u8 m_usedArgumentRegisters = 0;
        reg_set m_usedRegisters = 0;
    };
}