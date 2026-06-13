#pragma once

#include "compilation/function.h"
#include "ast/expression.h"
#include "DCHeader.h"
#include "DCScript.h"

namespace dconstruct::compilation {

    [[nodiscard]] program_binary_element function_context::to_binary_element() const noexcept {
        constexpr sid64 global_sid = SID("global");
        constexpr sid64 function_sid = SID("function");
        constexpr sid64 script_lambda_sid = SID("script-lambda");
        constexpr u64 deadbeef = 0xDEAD'BEEF'1337'F00D;

        program_binary_element element{get_size_in_bytes()};

        const sid64 name_id = std::holds_alternative<std::string>(m_name) ? SID(std::get<std::string>(m_name).c_str()) : std::get<sid64>(m_name);

        element.m_entry = {
            .m_nameID = name_id,
            .m_typeId = script_lambda_sid,
            .m_entryPtr = nullptr};

        element.push_bytes(script_lambda_sid, 0b0);
        const ScriptLambda lambda = {
            reinterpret_cast<u64*>(element.m_rawData.size() + sizeof(ScriptLambda)),
            reinterpret_cast<u64*>(element.m_rawData.size() + sizeof(ScriptLambda) + m_instructions.size() * sizeof(Instruction)),
            function_sid,
            get_scriptlambda_sum(),
            0x0,
            deadbeef,
            0x0,
            static_cast<u32>(m_instructions.size()),
            -1,
            global_sid,
            0x0};
        element.push_bytes(lambda, 0b0000'0011, 0b00);
        for (const Instruction& istr : m_instructions) {
            element.push_bytes(istr, 0b0);
        }
        for (u32 i = 0; i < m_symbolTable.size(); ++i) {
            if (m_symbolTableEntryPointers[i] == compilation::function_context::SYMBOL_TABLE_POINTER_KIND::STRING) {
                element.insert_string_offset();
                element.push_bytes(m_symbolTable[i], 0b1);
            } else if (m_symbolTableEntryPointers[i] == compilation::function_context::SYMBOL_TABLE_POINTER_KIND::ABSOLUTE) {
                element.insert_absolute_offset();
                element.push_bytes(m_symbolTable[i], 0b1);
            } else {
                element.push_bytes(m_symbolTable[i], 0b0);
            }
        }
        return element;
    }

    [[nodiscard]] resstr<reg_idx> function_context::get_next_unused_register() noexcept {
        u64 reg_set_num = m_usedRegisters.to_ullong();

        constexpr u64 all_50_bits_used = 0x3FFFFFFFFFFFFull;

        if ((reg_set_num & all_50_bits_used) == all_50_bits_used) {
            return std::unexpected{"no more free registers"};
        }

        reg_idx next_unused = std::countr_one(reg_set_num);
        m_usedRegisters.set(next_unused, true);
        return next_unused;
    }

    void function_context::free_register(const reg_idx reg) noexcept {
        if (!m_varsToRegs.value_used(reg)) {
            m_usedRegisters.set(reg, false);
        }
    }

    void function_context::free_lvalue_register(const reg_idx reg) noexcept {
        m_usedRegisters.set(reg, false);
    }

    errmsg function_context::save_used_argument_registers(const u8 count) noexcept {
        reg_set saved;
        for (u32 i = 0; i < count; ++i) {
            const resstr<reg_idx> reg = get_next_unused_register();
            if (!reg) {
                return reg.error();
            }
            emit_instruction(Opcode::Move, *reg, ARGUMENT_REGISTERS_IDX + i);
            saved.set(*reg, true);
            m_savedArgumentsTemporaryRegs.push_back(saved);
        }
        return std::nullopt;
    }

    void function_context::restore_used_argument_registers() noexcept {
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

    void function_context::emit_instruction(
        const Opcode opcode,
        const u8 destination,
        const u8 operand1,
        const u8 operand2
    ) noexcept {
        if (!m_deferred.empty()) {
            m_deferred.back().first.emplace_back(opcode, destination, operand1, operand2);
        } else {
            m_instructions.emplace_back(opcode, destination, operand1, operand2);
        }
    }

    void function_context::emit_lohi_instruction(const Opcode opcode, const u8 destination, const u16 lo_hi) noexcept {
        if (!m_deferred.empty()) {
            m_deferred.back().first.emplace_back(opcode, destination, lo_hi & 0xFF, (lo_hi >> 8) & 0xFF);
        } else {
            m_instructions.emplace_back(opcode, destination, lo_hi & 0xFF, (lo_hi >> 8) & 0xFF);
        }
    }

    [[nodiscard]] u16 function_context::add_to_symbol_table(
        const u64 value,
        const SYMBOL_TABLE_POINTER_KIND pointer_kind
    ) noexcept {
        const auto existing = std::find(m_symbolTable.begin(), m_symbolTable.end(), value);
        if (existing != m_symbolTable.end()) {
            return std::distance(m_symbolTable.begin(), existing);
        } else {
            const u16 current_size = m_symbolTable.size();
            m_symbolTable.push_back(value);
            m_symbolTableEntryPointers.push_back(pointer_kind);
            return current_size;
        }
    }

    [[nodiscard]] u64 function_context::get_size_in_bytes() const noexcept {
        return sizeof(sid64) + sizeof(ScriptLambda) + m_instructions.size() * sizeof(Instruction) + m_symbolTable.size() * sizeof(u64);
    }

    [[nodiscard]] u64 function_context::get_scriptlambda_sum() const noexcept {
        return 12 + 4 * (m_instructions.size() + m_symbolTable.size());
    }

    [[nodiscard]] resstr<reg_idx> function_context::fix_destination(const std::optional<reg_idx> passed_through_destination) noexcept {
        if (passed_through_destination) {
            return *passed_through_destination;
        } else {
            return get_next_unused_register();
        }
    }

    void function_context::push_deferred() noexcept {
        m_deferred.emplace_back(std::vector<Instruction>(), m_deferred.empty() ? m_instructions.size() : m_deferred.back().first.size());
    }

    void function_context::pop_deferred() noexcept {
        const auto [instructions, save_point] = std::move(m_deferred.back());
        m_deferred.pop_back();
        std::vector<Instruction>& next_back = m_deferred.empty() ? m_instructions : m_deferred.back().first;
        next_back.insert(next_back.begin() + save_point, instructions.begin(), instructions.end());
    }

}
