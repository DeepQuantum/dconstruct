#include "disassembly/instructions.h"
#include "string.h"

namespace dconstruct {
    const char* Instruction::opcode_to_string() const noexcept {
        switch (opcode) {
        case Opcode::Return: return "Return";
        case Opcode::IAdd: return "IAdd";
        case Opcode::ISub: return "ISub";
        case Opcode::IMul: return "IMul";
        case Opcode::IDiv: return "IDiv";
        case Opcode::FAdd: return "FAdd";
        case Opcode::FSub: return "FSub";
        case Opcode::FMul: return "FMul";
        case Opcode::FDiv: return "FDiv";
        case Opcode::LoadStaticInt: return "LoadStaticInt";
        case Opcode::LoadStaticFloat: return "LoadStaticFloat";
        case Opcode::LoadStaticPointer: return "LoadStaticPointer";
        case Opcode::LoadU16Imm: return "LoadU16Imm";
        case Opcode::LoadU32: return "LoadU32";
        case Opcode::LoadFloat: return "LoadFloat";
        case Opcode::LoadPointer: return "LoadPointer";
        case Opcode::StoreInt: return "StoreInt";
        case Opcode::StoreFloat: return "StoreFloat";
        case Opcode::StorePointer: return "StorePointer";
        case Opcode::LookupInt: return "LookupInt";
        case Opcode::LookupFloat: return "LookupFloat";
        case Opcode::LookupPointer: return "LookupPointer";
        case Opcode::MoveInt: return "MoveInt";
        case Opcode::MoveFloat: return "MoveFloat";
        case Opcode::MovePointer: return "MovePointer";
        case Opcode::CastInteger: return "CastInteger";
        case Opcode::CastFloat: return "CastFloat";
        case Opcode::Call: return "Call";
        case Opcode::CallFf: return "CallFf";
        case Opcode::IEqual: return "IEqual";
        case Opcode::IGreaterThan: return "IGreaterThan";
        case Opcode::IGreaterThanEqual: return "IGreaterThanEqual";
        case Opcode::ILessThan: return "ILessThan";
        case Opcode::ILessThanEqual: return "ILessThanEqual";
        case Opcode::FEqual: return "FEqual";
        case Opcode::FGreaterThan: return "FGreaterThan";
        case Opcode::FGreaterThanEqual: return "FGreaterThanEqual";
        case Opcode::FLessThan: return "FLessThan";
        case Opcode::FLessThanEqual: return "FLessThanEqual";
        case Opcode::IMod: return "IMod";
        case Opcode::FMod: return "FMod";
        case Opcode::IAbs: return "IAbs";
        case Opcode::FAbs: return "FAbs";
        case Opcode::GoTo: return "GoTo";
        case Opcode::Label: return "Label";
        case Opcode::Branch: return "Branch";
        case Opcode::BranchIf: return "BranchIf";
        case Opcode::BranchIfNot: return "BranchIfNot";
        case Opcode::OpLogNot: return "OpLogNot";
        case Opcode::OpBitAnd: return "OpBitAnd";
        case Opcode::OpBitNot: return "OpBitNot";
        case Opcode::OpBitOr: return "OpBitOr";
        case Opcode::OpBitXor: return "OpBitXor";
        case Opcode::OpBitNor: return "OpBitNor";
        case Opcode::OpLogAnd: return "OpLogAnd";
        case Opcode::OpLogOr: return "OpLogOr";
        case Opcode::INeg: return "INeg";
        case Opcode::FNeg: return "FNeg";
        case Opcode::LoadParamCnt: return "LoadParamCnt";
        case Opcode::IAddImm: return "IAddImm";
        case Opcode::ISubImm: return "ISubImm";
        case Opcode::IMulImm: return "IMulImm";
        case Opcode::IDivImm: return "IDivImm";
        case Opcode::LoadStaticI32Imm: return "LoadStaticI32Imm";
        case Opcode::LoadStaticFloatImm: return "LoadStaticFloatImm";
        case Opcode::LoadStaticPointerImm: return "LoadStaticPointerImm";
        case Opcode::IntAsh: return "IntAsh";
        case Opcode::Move: return "Move";
        case Opcode::LoadStaticU32Imm: return "LoadStaticU32Imm";
        case Opcode::LoadStaticI8Imm: return "LoadStaticI8Imm";
        case Opcode::LoadStaticU8Imm: return "LoadStaticU8Imm";
        case Opcode::LoadStaticI16Imm: return "LoadStaticI16Imm";
        case Opcode::LoadStaticU16Imm: return "LoadStaticU16Imm";
        case Opcode::LoadStaticI64Imm: return "LoadStaticI64Imm";
        case Opcode::LoadStaticU64Imm: return "LoadStaticU64Imm";
        case Opcode::LoadI8: return "LoadI8";
        case Opcode::LoadU8: return "LoadU8";
        case Opcode::LoadI16: return "LoadI16";
        case Opcode::LoadU16: return "LoadU16";
        case Opcode::LoadI32: return "LoadI32";
        case Opcode::LoadI64: return "LoadI64";
        case Opcode::LoadU64: return "LoadU64";
        case Opcode::StoreI8: return "StoreI8";
        case Opcode::StoreU8: return "StoreU8";
        case Opcode::StoreI16: return "StoreI16";
        case Opcode::StoreU16: return "StoreU16";
        case Opcode::StoreI32: return "StoreI32";
        case Opcode::StoreU32: return "StoreU32";
        case Opcode::StoreI64: return "StoreI64";
        case Opcode::StoreU64: return "StoreU64";
        case Opcode::INotEqual: return "INotEqual";
        case Opcode::FNotEqual: return "FNotEqual";
        case Opcode::StoreArray: return "StoreArray";
        case Opcode::AssertPointer: return "AssertPointer";
        case Opcode::BreakFlag: return "BreakFlag";
        case Opcode::Breakpoint: return "Breakpoint";
        default: return "Unknown Opcode";
        }
    }

    [[nodiscard]] b8 Instruction::destination_is_immediate() const noexcept {
        return opcode == Opcode::Branch || opcode == Opcode::LoadU16Imm;
    }

    void StackFrame::to_string(char* buffer, const u64 buffer_size, const u64 idx, const char* resolved) const noexcept {
        const Register& reg = m_registers[idx];
        if (reg.isArg) {
            std::snprintf(buffer, buffer_size, "arg_%i", reg.argNum);
            return;
        }
        switch (reg.m_type) {
        case RegisterValueType::HASH: {
            strncpy(buffer, resolved, buffer_size);
            break;
        }
        case RegisterValueType::U16:
        case RegisterValueType::U32:
        case RegisterValueType::U64: {
            std::snprintf(buffer, buffer_size, "%llu", reg.m_U64);
            break;
        }
        case RegisterValueType::I16:
        case RegisterValueType::I32: {
            std::snprintf(buffer, buffer_size, "%i", reg.m_I32);
            break;
        }
        case RegisterValueType::I64: {
            std::snprintf(buffer, buffer_size, "%lli", reg.m_I64);
            break;
        }
        case RegisterValueType::F16:
        case RegisterValueType::F32:
        case RegisterValueType::F64:
            std::snprintf(buffer, buffer_size, "%.2f", reg.m_F32);
            break;
        case RegisterValueType::STRING:
            std::snprintf(buffer, buffer_size, "\"%s\"", reinterpret_cast<const char*>(reg.m_PTR.get()));
            break;
        case RegisterValueType::POINTER: {
            if (reg.m_PTR.m_offset > 0) {
                std::snprintf(buffer, buffer_size, "[%s%s + %llu]", resolved, reg.isReturn ? "RET_" : "", reg.m_PTR.m_offset);
            }
            else {
                if (reg.isReturn) {
                    std::snprintf(buffer, buffer_size, "RET_%s", resolved);
                } else {
                    std::snprintf(buffer, buffer_size, "%s", resolved);
                }
            }
            break;
        }
        default: {
            std::snprintf(buffer, buffer_size, "0x%llX", reg.m_U64);
            break;
        }
        }
    }

    Register& StackFrame::operator[](const u64 idx) noexcept {
        return m_registers[idx];
    }
}