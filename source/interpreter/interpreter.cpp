#include "interpreter.h"

#include <cmath>

namespace dconstruct::interpreter {
    u64 interpreter::run(const std::vector<Instruction>& instructions, const std::vector<u64>& symbol_table) noexcept {
        for (u64 ip = 0; ip < instructions.size();) {
            const auto& instruction = instructions[ip];
            const auto destination = instruction.destination;
            const auto operand1 = instruction.operand1;
            const auto operand2 = instruction.operand2;
            bool advance = true;

            switch (instruction.opcode) {
                case Opcode::Return: {
                    return m_stackFrame[destination];
                }
                case Opcode::IAdd: {
                    m_stackFrame[destination] = m_stackFrame[operand1] + m_stackFrame[operand2];
                    break;
                }
                case Opcode::ISub: {
                    m_stackFrame[destination] = m_stackFrame[operand1] - m_stackFrame[operand2];
                    break;
                }
                case Opcode::IMul: {
                    m_stackFrame[destination] = m_stackFrame[operand1] * m_stackFrame[operand2];
                    break;
                }
                case Opcode::IDiv: {
                    m_stackFrame[destination] = static_cast<u64>(static_cast<i64>(m_stackFrame[operand1]) / static_cast<i64>(m_stackFrame[operand2]));
                    break;
                }
                case Opcode::FAdd: {
                    const f32 value = std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand1])) + std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand2]));
                    m_stackFrame[destination] = std::bit_cast<u32>(value);
                    break;
                }
                case Opcode::FSub: {
                    const f32 value = std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand1])) - std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand2]));
                    m_stackFrame[destination] = std::bit_cast<u32>(value);
                    break;
                }
                case Opcode::FMul: {
                    const f32 value = std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand1])) * std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand2]));
                    m_stackFrame[destination] = std::bit_cast<u32>(value);
                    break;
                }
                case Opcode::FDiv: {
                    const f32 value = std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand1])) / std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand2]));
                    m_stackFrame[destination] = std::bit_cast<u32>(value);
                    break;
                }
                case Opcode::LoadStaticInt:
                case Opcode::LoadStaticPointer: {
                    m_stackFrame[destination] = symbol_table[m_stackFrame[operand1]];
                    break;
                }
                case Opcode::LoadStaticFloat: {
                    m_stackFrame[destination] = static_cast<u32>(symbol_table[m_stackFrame[operand1]]);
                    break;
                }
                case Opcode::LoadU16Imm: {
                    m_stackFrame[destination] = operand1 | (static_cast<u16>(operand2) << 8);
                    break;
                }
                case Opcode::LoadU32: {
                    m_stackFrame[destination] = *reinterpret_cast<const u32*>(m_stackFrame[operand1]);
                    break;
                }
                case Opcode::LoadFloat: {
                    m_stackFrame[destination] = std::bit_cast<u32>(*reinterpret_cast<const f32*>(m_stackFrame[operand1]));
                    break;
                }
                case Opcode::LoadPointer: {
                    m_stackFrame[destination] = *reinterpret_cast<const u64*>(m_stackFrame[operand1]);
                    break;
                }
                case Opcode::StoreInt:
                case Opcode::StoreI32: {
                    *reinterpret_cast<i32*>(m_stackFrame[operand1]) = static_cast<i32>(m_stackFrame[operand2]);
                    m_stackFrame[destination] = m_stackFrame[operand1];
                    break;
                }
                case Opcode::StoreFloat: {
                    *reinterpret_cast<f32*>(m_stackFrame[operand1]) = std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand2]));
                    m_stackFrame[destination] = m_stackFrame[operand1];
                    break;
                }
                case Opcode::StorePointer:
                case Opcode::StoreU64: {
                    *reinterpret_cast<u64*>(m_stackFrame[operand1]) = m_stackFrame[operand2];
                    m_stackFrame[destination] = m_stackFrame[operand1];
                    break;
                }
                case Opcode::LookupInt:
                case Opcode::LookupPointer: {
                    m_stackFrame[destination] = symbol_table[operand1];
                    break;
                }
                case Opcode::LookupFloat: {
                    m_stackFrame[destination] = static_cast<u32>(symbol_table[operand1]);
                    break;
                }
                case Opcode::MoveInt:
                case Opcode::MoveFloat:
                case Opcode::MovePointer:
                case Opcode::Move: {
                    m_stackFrame[destination] = m_stackFrame[operand1];
                    break;
                }
                case Opcode::CastInteger: {
                    m_stackFrame[destination] = static_cast<u64>(static_cast<i64>(static_cast<i32>(std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand1])))));
                    break;
                }
                case Opcode::CastFloat: {
                    const f32 value = static_cast<f32>(static_cast<i64>(m_stackFrame[operand1]));
                    m_stackFrame[destination] = std::bit_cast<u32>(value);
                    break;
                }
                case Opcode::Call:
                case Opcode::CallFf: {
                    break;
                }
                case Opcode::IEqual: {
                    m_stackFrame[destination] = m_stackFrame[operand1] == m_stackFrame[operand2];
                    break;
                }
                case Opcode::IGreaterThan: {
                    m_stackFrame[destination] = static_cast<i64>(m_stackFrame[operand1]) > static_cast<i64>(m_stackFrame[operand2]);
                    break;
                }
                case Opcode::IGreaterThanEqual: {
                    m_stackFrame[destination] = static_cast<i64>(m_stackFrame[operand1]) >= static_cast<i64>(m_stackFrame[operand2]);
                    break;
                }
                case Opcode::ILessThan: {
                    m_stackFrame[destination] = static_cast<i64>(m_stackFrame[operand1]) < static_cast<i64>(m_stackFrame[operand2]);
                    break;
                }
                case Opcode::ILessThanEqual: {
                    m_stackFrame[destination] = static_cast<i64>(m_stackFrame[operand1]) <= static_cast<i64>(m_stackFrame[operand2]);
                    break;
                }
                case Opcode::FEqual: {
                    m_stackFrame[destination] = std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand1])) == std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand2]));
                    break;
                }
                case Opcode::FGreaterThan: {
                    m_stackFrame[destination] = std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand1])) > std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand2]));
                    break;
                }
                case Opcode::FGreaterThanEqual: {
                    m_stackFrame[destination] = std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand1])) >= std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand2]));
                    break;
                }
                case Opcode::FLessThan: {
                    m_stackFrame[destination] = std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand1])) < std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand2]));
                    break;
                }
                case Opcode::FLessThanEqual: {
                    m_stackFrame[destination] = std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand1])) <= std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand2]));
                    break;
                }
                case Opcode::IMod: {
                    m_stackFrame[destination] = static_cast<u64>(static_cast<i64>(m_stackFrame[operand1]) % static_cast<i64>(m_stackFrame[operand2]));
                    break;
                }
                case Opcode::FMod: {
                    const f32 value = std::fmod(std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand1])), std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand2])));
                    m_stackFrame[destination] = std::bit_cast<u32>(value);
                    break;
                }
                case Opcode::IAbs: {
                    const i64 value = static_cast<i64>(m_stackFrame[operand1]);
                    m_stackFrame[destination] = value < 0 ? static_cast<u64>(-value) : static_cast<u64>(value);
                    break;
                }
                case Opcode::FAbs: {
                    const f32 value = std::fabs(std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand1])));
                    m_stackFrame[destination] = std::bit_cast<u32>(value);
                    break;
                }
                case Opcode::GoTo:
                case Opcode::Branch: {
                    ip = destination | (static_cast<u16>(operand2) << 8);
                    advance = false;
                    break;
                }
                case Opcode::Label: {
                    break;
                }
                case Opcode::BranchIf: {
                    if (m_stackFrame[operand1] != 0) {
                        ip = destination | (static_cast<u16>(operand2) << 8);
                        advance = false;
                    }
                    break;
                }
                case Opcode::BranchIfNot: {
                    if (m_stackFrame[operand1] == 0) {
                        ip = destination | (static_cast<u16>(operand2) << 8);
                        advance = false;
                    }
                    break;
                }
                case Opcode::OpLogNot: {
                    m_stackFrame[destination] = m_stackFrame[operand1] == 0;
                    break;
                }
                case Opcode::OpBitAnd: {
                    m_stackFrame[destination] = m_stackFrame[operand1] & m_stackFrame[operand2];
                    break;
                }
                case Opcode::OpBitNot: {
                    m_stackFrame[destination] = ~m_stackFrame[operand1];
                    break;
                }
                case Opcode::OpBitOr: {
                    m_stackFrame[destination] = m_stackFrame[operand1] | m_stackFrame[operand2];
                    break;
                }
                case Opcode::OpBitXor: {
                    m_stackFrame[destination] = m_stackFrame[operand1] ^ m_stackFrame[operand2];
                    break;
                }
                case Opcode::OpBitNor: {
                    m_stackFrame[destination] = ~(m_stackFrame[operand1] | m_stackFrame[operand2]);
                    break;
                }
                case Opcode::OpLogAnd: {
                    m_stackFrame[destination] = m_stackFrame[operand1] != 0 && m_stackFrame[operand2] != 0;
                    break;
                }
                case Opcode::OpLogOr: {
                    m_stackFrame[destination] = m_stackFrame[operand1] != 0 || m_stackFrame[operand2] != 0;
                    break;
                }
                case Opcode::INeg: {
                    m_stackFrame[destination] = static_cast<u64>(-static_cast<i64>(m_stackFrame[operand1]));
                    break;
                }
                case Opcode::FNeg: {
                    const f32 value = -std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand1]));
                    m_stackFrame[destination] = std::bit_cast<u32>(value);
                    break;
                }
                case Opcode::LoadParamCnt: {
                    m_stackFrame[destination] = operand1;
                    break;
                }
                case Opcode::IAddImm: {
                    m_stackFrame[destination] = m_stackFrame[operand1] + operand2;
                    break;
                }
                case Opcode::ISubImm: {
                    m_stackFrame[destination] = m_stackFrame[operand1] - operand2;
                    break;
                }
                case Opcode::IMulImm: {
                    m_stackFrame[destination] = m_stackFrame[operand1] * operand2;
                    break;
                }
                case Opcode::IDivImm: {
                    m_stackFrame[destination] = static_cast<u64>(static_cast<i64>(m_stackFrame[operand1]) / operand2);
                    break;
                }
                case Opcode::LoadStaticI32Imm: {
                    m_stackFrame[destination] = static_cast<u64>(static_cast<i64>(static_cast<i32>(symbol_table[operand1])));
                    break;
                }
                case Opcode::LoadStaticFloatImm: {
                    m_stackFrame[destination] = static_cast<u32>(symbol_table[operand1]);
                    break;
                }
                case Opcode::LoadStaticPointerImm:
                case Opcode::LoadStaticU64Imm: {
                    m_stackFrame[destination] = symbol_table[operand1];
                    break;
                }
                case Opcode::IntAsh: {
                    const i8 shift = static_cast<i8>(m_stackFrame[operand2]);
                    if (shift >= 0) {
                        m_stackFrame[destination] = static_cast<u64>(static_cast<i64>(m_stackFrame[operand1]) << shift);
                    } else {
                        m_stackFrame[destination] = static_cast<u64>(static_cast<i64>(m_stackFrame[operand1]) >> -shift);
                    }
                    break;
                }
                case Opcode::LoadStaticU32Imm: {
                    m_stackFrame[destination] = static_cast<u32>(symbol_table[operand1]);
                    break;
                }
                case Opcode::LoadStaticI8Imm: {
                    m_stackFrame[destination] = static_cast<u64>(static_cast<i64>(static_cast<i8>(symbol_table[operand1])));
                    break;
                }
                case Opcode::LoadStaticU8Imm: {
                    m_stackFrame[destination] = static_cast<u8>(symbol_table[operand1]);
                    break;
                }
                case Opcode::LoadStaticI16Imm: {
                    m_stackFrame[destination] = static_cast<u64>(static_cast<i64>(static_cast<i16>(symbol_table[operand1])));
                    break;
                }
                case Opcode::LoadStaticU16Imm: {
                    m_stackFrame[destination] = static_cast<u16>(symbol_table[operand1]);
                    break;
                }
                case Opcode::LoadStaticI64Imm: {
                    m_stackFrame[destination] = symbol_table[operand1];
                    break;
                }
                case Opcode::LoadI8: {
                    m_stackFrame[destination] = static_cast<u64>(static_cast<i64>(*reinterpret_cast<const i8*>(m_stackFrame[operand1])));
                    break;
                }
                case Opcode::LoadU8: {
                    m_stackFrame[destination] = *reinterpret_cast<const u8*>(m_stackFrame[operand1]);
                    break;
                }
                case Opcode::LoadI16: {
                    m_stackFrame[destination] = static_cast<u64>(static_cast<i64>(*reinterpret_cast<const i16*>(m_stackFrame[operand1])));
                    break;
                }
                case Opcode::LoadU16: {
                    m_stackFrame[destination] = *reinterpret_cast<const u16*>(m_stackFrame[operand1]);
                    break;
                }
                case Opcode::LoadI32: {
                    m_stackFrame[destination] = static_cast<u64>(static_cast<i64>(*reinterpret_cast<const i32*>(m_stackFrame[operand1])));
                    break;
                }
                case Opcode::LoadI64: {
                    m_stackFrame[destination] = *reinterpret_cast<const i64*>(m_stackFrame[operand1]);
                    break;
                }
                case Opcode::LoadU64: {
                    m_stackFrame[destination] = *reinterpret_cast<const u64*>(m_stackFrame[operand1]);
                    break;
                }
                case Opcode::StoreI8: {
                    *reinterpret_cast<i8*>(m_stackFrame[operand1]) = static_cast<i8>(m_stackFrame[operand2]);
                    m_stackFrame[destination] = m_stackFrame[operand1];
                    break;
                }
                case Opcode::StoreU8: {
                    *reinterpret_cast<u8*>(m_stackFrame[operand1]) = static_cast<u8>(m_stackFrame[operand2]);
                    m_stackFrame[destination] = m_stackFrame[operand1];
                    break;
                }
                case Opcode::StoreI16: {
                    *reinterpret_cast<i16*>(m_stackFrame[operand1]) = static_cast<i16>(m_stackFrame[operand2]);
                    m_stackFrame[destination] = m_stackFrame[operand1];
                    break;
                }
                case Opcode::StoreU16: {
                    *reinterpret_cast<u16*>(m_stackFrame[operand1]) = static_cast<u16>(m_stackFrame[operand2]);
                    m_stackFrame[destination] = m_stackFrame[operand1];
                    break;
                }
                case Opcode::StoreU32: {
                    *reinterpret_cast<u32*>(m_stackFrame[operand1]) = static_cast<u32>(m_stackFrame[operand2]);
                    m_stackFrame[destination] = m_stackFrame[operand1];
                    break;
                }
                case Opcode::StoreI64: {
                    *reinterpret_cast<i64*>(m_stackFrame[operand1]) = static_cast<i64>(m_stackFrame[operand2]);
                    m_stackFrame[destination] = m_stackFrame[operand1];
                    break;
                }
                case Opcode::INotEqual: {
                    m_stackFrame[destination] = m_stackFrame[operand1] != m_stackFrame[operand2];
                    break;
                }
                case Opcode::FNotEqual: {
                    m_stackFrame[destination] = std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand1])) != std::bit_cast<f32>(static_cast<u32>(m_stackFrame[operand2]));
                    break;
                }
                case Opcode::StoreArray: {
                    break;
                }
                case Opcode::AssertPointer: {
                    break;
                }
                case Opcode::BreakFlag:
                case Opcode::Breakpoint: {
                    return m_stackFrame[0];
                }
                case Opcode::QEX_InRangeI: {
                    const u64 value = m_stackFrame[operand1] - symbol_table[operand2];
                    const u64 range = symbol_table[operand2 + 1] - symbol_table[operand2];
                    m_stackFrame[destination] = value <= range;
                    break;
                }
                default: {
                    return 0;
                }
            }

            if (advance) {
                ++ip;
            }
        }

        return m_stackFrame[0];
    }
}
