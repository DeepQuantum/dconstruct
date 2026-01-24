#pragma once

#include "compilation/function.h"
#include "ast/expression.h"


namespace dconstruct::compiler {
[[nodiscard]] std::optional<reg_idx> function::get_next_unused_register() noexcept {
    const u64 reg_set_num = m_usedRegisters.to_ullong();

    constexpr u64 all_50_bits_used = 0x3FFFFFFFFFFFFull;

    if (reg_set_num >= all_50_bits_used) {
        return std::nullopt;
    }

    reg_idx next_unused = std::countr_zero(reg_set_num);
    m_usedRegisters.set(next_unused, true);
    return next_unused;
}

void function::free_register(const ast::expression& expr, const reg_idx reg) noexcept {
    if (!expr.is_l_evaluable()) {
        m_usedRegisters.set(reg, false);
    }
}

void function::emit_instruction(const Opcode opcode, const u8 destination, const u8 operand1, const u8 operand2) noexcept {
    m_instructions.emplace_back(opcode, destination, operand1, operand2);
}


[[nodiscard]] u8 function::add_to_symbol_table(const u64 value, const SYMBOL_TABLE_POINTER_KIND pointer_kind) noexcept {
    const u8 current_size = m_symbolTable.size();
    m_symbolTable.push_back(value);
    m_symbolTableEntryPointers.push_back(pointer_kind);
    return current_size;
}

[[nodiscard]] u64 function::get_size_in_bytes() const noexcept {
    return m_instructions.size() * sizeof(Instruction) + m_symbolTable.size() * sizeof(u64);
}

[[nodiscard]] u64 function::get_scriptlambda_sum() const noexcept {
    return 12 + (4 * m_instructions.size()) + (4 * m_symbolTable.size());
}

}