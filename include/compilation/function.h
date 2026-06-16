#pragma once

#include "base.h"
#include "disassembly/instructions.h"
#include "environment.h"
#include "program_binary_element.h"
#include <unordered_map>

namespace dconstruct::ast {
    struct expression;
}

namespace dconstruct::compilation {
    struct function_context {
        static constexpr u8 BRANCH_PLACEHOLDER = 0xFF;

        enum class SYMBOL_TABLE_POINTER_KIND {
            NONE,
            STRING,
            ABSOLUTE,
        };

        function_context() noexcept = default;

        function_context(const function_context& rhs) noexcept :
            m_instructions(rhs.m_instructions),
            m_symbolTable(rhs.m_symbolTable),
            m_symbolTableEntryPointers(rhs.m_symbolTableEntryPointers),
            m_name(rhs.m_name) {}

        function_context& operator=(const function_context& rhs) noexcept {
            m_instructions = rhs.m_instructions;
            m_symbolTable = rhs.m_symbolTable;
            m_symbolTableEntryPointers = rhs.m_symbolTableEntryPointers;
            m_name = rhs.m_name;
            return *this;
        }

        function_context(function_context&& rhs) noexcept = default;
        function_context& operator=(function_context&& rhs) noexcept = default;

        [[nodiscard]] program_binary_element to_binary_element() const noexcept;

        void emit_instruction(const Opcode opcode, const u8 destination, const u8 operand1 = 0, const u8 operand2 = 0) noexcept;
        void emit_lohi_instruction(const Opcode opcode, const u8 destination, const u16 lo_hi) noexcept;

        [[nodiscard]] resstr<reg_idx> get_next_unused_register() noexcept;
        void free_register(const reg_idx reg) noexcept;
        void free_lvalue_register(const reg_idx reg) noexcept;

        errmsg save_used_argument_registers(const u8 count) noexcept;
        void restore_used_argument_registers() noexcept;

        [[nodiscard]] u16 add_to_symbol_table(const u64 value, const SYMBOL_TABLE_POINTER_KIND pointer_kind = SYMBOL_TABLE_POINTER_KIND::NONE) noexcept;

        [[nodiscard]] u64 get_size_in_bytes() const noexcept;
        [[nodiscard]] u64 get_scriptlambda_sum() const noexcept;
        [[nodiscard]] resstr<reg_idx> fix_destination(const std::optional<u8> passed_through_destination) noexcept;
        void push_deferred() noexcept;
        void pop_deferred() noexcept;

        std::vector<Instruction> m_instructions;
        std::vector<u64> m_symbolTable;
        std::vector<SYMBOL_TABLE_POINTER_KIND> m_symbolTableEntryPointers;
        std::vector<u8> m_numberSavedArgumentsStack;
        std::vector<reg_set> m_savedArgumentsTemporaryRegs;
        std::vector<u16> m_returnBranchLocations;
        std::vector<std::pair<std::vector<Instruction>, u64>> m_deferred;
        environment<reg_idx> m_varsToRegs;
        std::variant<std::string, sid64> m_name;
        u16 m_deferredPoint;
        u8 m_usedArgumentRegisters = 0;
        reg_set m_usedRegisters = 0;
    };
}
