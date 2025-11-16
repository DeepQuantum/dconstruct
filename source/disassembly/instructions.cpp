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

    [[nodiscard]] bool Instruction::destination_is_immediate() const noexcept {
        return opcode == Opcode::Branch || opcode == Opcode::BranchIf
            || opcode == Opcode::BranchIfNot;
    }

    [[nodiscard]] bool Instruction::operand1_is_immediate() const noexcept {
        return opcode == Opcode::LoadStaticI32Imm || opcode == Opcode::LoadStaticI16Imm
            || opcode == Opcode::LoadStaticU16Imm || opcode == Opcode::LoadStaticI8Imm
            || opcode == Opcode::LoadStaticU8Imm || opcode == Opcode::LoadStaticU32Imm
            || opcode == Opcode::LoadStaticI64Imm || opcode == Opcode::LoadStaticU64Imm
            || opcode == Opcode::LoadStaticFloatImm || opcode == Opcode::LoadStaticPointer
            || opcode == Opcode::LoadU16Imm || opcode == Opcode::LookupPointer;
    }

    [[nodiscard]] bool Instruction::operand2_is_immediate() const noexcept {
        return opcode == Opcode::Call || opcode == Opcode::CallFf
            || opcode == Opcode::IAddImm || opcode == Opcode::IMulImm
            || opcode == Opcode::ISubImm || opcode == Opcode::IDivImm
            || opcode == Opcode::LoadU16Imm;
    }

    [[nodiscard]] bool Instruction::operand1_is_used() const noexcept {
        return opcode != Opcode::Branch && opcode != Opcode::Return && opcode != Opcode::AssertPointer;
    }

    [[nodiscard]] bool Instruction::operand2_is_used() const noexcept {
        u32 num = static_cast<u32>(opcode);
        bool is_arithmetic = num >= static_cast<u32>(Opcode::IAdd) && num <= static_cast<u32>(Opcode::FDiv);
        bool is_call = opcode == Opcode::CallFf || opcode == Opcode::Call;
        bool is_comp = num >= static_cast<u32>(Opcode::IEqual) && num <= static_cast<u32>(Opcode::FMod);
        bool is_bit = opcode == Opcode::OpBitAnd || (num >= static_cast<u32>(Opcode::OpBitOr) && num <= static_cast<u32>(Opcode::OpLogOr));
        bool is_arithmetic_imm = num >= static_cast<u32>(Opcode::IAddImm) && num <= static_cast<u32>(Opcode::IDivImm);
        bool is_store = num >= static_cast<u32>(Opcode::StoreI8) && num <= static_cast<u32>(Opcode::FNotEqual);
        return is_arithmetic || is_call || is_comp || is_bit || is_arithmetic_imm || is_store;
    }

    [[nodiscard]] bool Instruction::op1_is_reg() const noexcept {
        return operand1_is_used() && !operand1_is_immediate();
    }

    [[nodiscard]] bool Instruction::op2_is_reg() const noexcept {
        return operand2_is_used() && !operand2_is_immediate();
    }

    void StackFrame::to_string(char* buffer, const u64 buffer_size, const u64 idx, const char* resolved) const noexcept {
        const Register& reg = m_registers[idx];
        if (reg.m_containsArg) {
            std::snprintf(buffer, buffer_size, "arg_%i", reg.m_argNum);
        } else if (std::holds_alternative<ast::primitive_type>(reg.m_type)) {
            switch(std::get<ast::primitive_type>(reg.m_type).m_type) {
                case ast::primitive_kind::SID: {
                    strncpy(buffer, resolved, buffer_size);
                    break;
                } 
                case ast::primitive_kind::STRING: {
                    std::snprintf(buffer, buffer_size, "\"%s\"", reinterpret_cast<const char*>(reg.m_value));
                    break;
                }                      
                default: {
                    std::snprintf(buffer, buffer_size, "%llu", reg.m_value);
                }
            }
        } else if (reg.is_pointer()) {
            std::snprintf(buffer, buffer_size, "[%s%s + %u]", reg.m_isReturn ? "RET_" : "", resolved, reg.m_pointerOffset);
        } else if (reg.m_isReturn) {
            std::snprintf(buffer, buffer_size, "RET_%s", resolved);
        } else {
            std::snprintf(buffer, buffer_size, "%s", resolved);
        }
    }

    Register& StackFrame::operator[](const u64 idx) noexcept {
        return m_registers[idx];
    }
}