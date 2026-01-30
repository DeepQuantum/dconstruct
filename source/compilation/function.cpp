#pragma once

#include "compilation/function.h"
#include "ast/expression.h"


namespace dconstruct::compilation {
[[nodiscard]] std::expected<reg_idx, std::string> function::get_next_unused_register() noexcept {
    u64 reg_set_num = m_usedRegisters.to_ullong();

    constexpr u64 all_50_bits_used = 0x3FFFFFFFFFFFFull;

    if (reg_set_num >= all_50_bits_used) {
        return std::unexpected{"no more free registers"};
    }

    reg_idx next_unused = std::countr_one(reg_set_num);
    m_usedRegisters.set(next_unused, true);
    return next_unused;
}

void function::free_register(const reg_idx reg) noexcept {
    if (!m_varsToRegs.value_used(reg)) {
        m_usedRegisters.set(reg, false);
    }
}

void function::free_lvalue_register(const reg_idx reg) noexcept {
    m_usedRegisters.set(reg, false);
}

std::optional<std::string> function::save_used_argument_registers(const u8 count) noexcept {
    reg_set saved;
    for (u32 i = 0; i < count; ++i) {
        const std::expected<reg_idx, std::string> reg = get_next_unused_register();
        if (!reg) {
            return reg.error();
        }
        emit_instruction(Opcode::Move, *reg, ARGUMENT_REGISTERS_IDX + i);
        saved.set(*reg, true);
        m_savedArgumentsTemporaryRegs.push_back(saved);
    }
    return std::nullopt;
}

void function::restore_used_argument_registers() noexcept {
    const reg_set regs = m_savedArgumentsTemporaryRegs.back();
    m_savedArgumentsTemporaryRegs.pop_back();
    u64 num = regs.to_ullong();
    u8 start = ARGUMENT_REGISTERS_IDX;
    u8 i = 0;
    while (num != 0) {
        const u8 next_reg = std::countr_zero(num);
        num &= num - 1;
        emit_instruction(Opcode::Move, start + i++, next_reg);
    }
}


void function::emit_instruction(const Opcode opcode, const u8 destination, const u8 operand1, const u8 operand2) noexcept {
    if (!m_deferred.empty()) {
        m_deferred.back().emplace_back(opcode, destination, operand1, operand2);
    } else {
        m_instructions.emplace_back(opcode, destination, operand1, operand2);
    }
}


[[nodiscard]] u8 function::add_to_symbol_table(const u64 value, const SYMBOL_TABLE_POINTER_KIND pointer_kind) noexcept {
    const auto existing = std::find(m_symbolTable.begin(), m_symbolTable.end(), value);
    if (existing != m_symbolTable.end()) {
        return std::distance(m_symbolTable.begin(), existing);
    } else {
        const u8 current_size = m_symbolTable.size();
        m_symbolTable.push_back(value);
        m_symbolTableEntryPointers.push_back(pointer_kind);
        return current_size;
    }
}

[[nodiscard]] u64 function::get_size_in_bytes() const noexcept {
    return m_instructions.size() * sizeof(Instruction) + m_symbolTable.size() * sizeof(u64);
}

[[nodiscard]] u64 function::get_scriptlambda_sum() const noexcept {
    return 12 + 4 * (m_instructions.size() + m_symbolTable.size());
}

[[nodiscard]] emission_res function::get_destination(const std::optional<reg_idx> passed_through_destination) noexcept {
    if (passed_through_destination) {
        return *passed_through_destination;
    } else {
        return get_next_unused_register();
    }
}

void function::push_deferred() noexcept {
    m_deferred.push_back({});
}

void function::pop_deferred() noexcept {
    std::vector<Instruction> back = std::move(m_deferred.back());
    m_deferred.pop_back();
    std::vector<Instruction>& next_back = m_deferred.empty() ? m_instructions : m_deferred.back();
    next_back.insert(next_back.begin(), back.begin(), back.end());
}

}