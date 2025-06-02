#include "instructions.h"
#include "string.h"

std::string Instruction::opcodeToString() const noexcept {
    switch (this->opcode) {
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

b8 Instruction::isSymbolLoadInstruction() const noexcept {
    Opcode op = this->opcode;
    return (op > LoadStaticU32Imm && op < LoadU64) || 
    op == LoadStaticI32Imm || op == LoadStaticFloatImm || op == LoadStaticPointerImm || 
    op == LookupInt ||op == LookupFloat || op == LookupPointer;
}

b8 Instruction::isBranchInstruction() const noexcept {
    return this->opcode == Branch || this->opcode == BranchIf || this->opcode == BranchIfNot;
}

std::string StackFrame::toString(const u64 idx, const std::string &resolved) const noexcept {
    char out[128] = {0};
    const Register reg = this->registers[idx];
    switch (reg.m_type) {
        case RegisterValueType::R_HASH: {
            sprintf(out, "%s", resolved.c_str());
            break;
        }
        case RegisterValueType::R_U16: 
        case RegisterValueType::R_U32: 
        case RegisterValueType::R_U64:
        case RegisterValueType::R_I16:
        case RegisterValueType::R_I32:
        case RegisterValueType::R_I64: {
            sprintf(out, "%i", reg.m_U64);
            break;
        }
        default: {
            sprintf(out, "0x%X", reg.m_U64);
            break;
        } 
    }
    if (reg.isReturn) {
        sprintf(out + strlen(out), "()");
    }
    return std::string(out);
}

Register& StackFrame::operator[](const u64 idx) noexcept {
    return this->registers[idx];
}