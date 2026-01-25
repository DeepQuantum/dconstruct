
#pragma once
#include "base.h"
#include "ast/type.h"
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <array>
#include <limits>


namespace dconstruct {
    enum class Opcode : u8 {
        Return,
        IAdd,
        ISub,
        IMul,
        IDiv,
        FAdd,
        FSub,
        FMul,
        FDiv,
        LoadStaticInt,
        LoadStaticFloat,
        LoadStaticPointer,
        LoadU16Imm,
        LoadU32,
        LoadFloat,
        LoadPointer,
        StoreInt,
        StoreFloat,
        StorePointer,
        LookupInt,
        LookupFloat,
        LookupPointer,
        MoveInt,
        MoveFloat,
        MovePointer,
        CastInteger,
        CastFloat,
        Call,
        CallFf,
        IEqual,
        IGreaterThan,
        IGreaterThanEqual,
        ILessThan,
        ILessThanEqual,
        FEqual,
        FGreaterThan,
        FGreaterThanEqual,
        FLessThan,
        FLessThanEqual,
        IMod,
        FMod,
        IAbs,
        FAbs,
        GoTo,
        Label,
        Branch,
        BranchIf,
        BranchIfNot,
        OpLogNot,
        OpBitAnd,
        OpBitNot,
        OpBitOr,
        OpBitXor,
        OpBitNor,
        OpLogAnd,
        OpLogOr,
        INeg,
        FNeg,
        LoadParamCnt,
        IAddImm,
        ISubImm,
        IMulImm,
        IDivImm,
        LoadStaticI32Imm,
        LoadStaticFloatImm,
        LoadStaticPointerImm,
        IntAsh,
        Move,
        LoadStaticU32Imm,
        LoadStaticI8Imm,
        LoadStaticU8Imm,
        LoadStaticI16Imm,
        LoadStaticU16Imm,
        LoadStaticI64Imm,
        LoadStaticU64Imm,
        LoadI8,
        LoadU8,
        LoadI16,
        LoadU16,
        LoadI32,
        LoadI64,
        LoadU64,
        StoreI8,
        StoreU8,
        StoreI16,
        StoreU16,
        StoreI32,
        StoreU32,
        StoreI64,
        StoreU64,
        INotEqual,
        FNotEqual,
        StoreArray,
        AssertPointer,
        BreakFlag,
        Breakpoint,
    };


template<u8 padding>
struct UP_Instruction {
    Opcode opcode;
    u8 destination;
    u8 operand1;
    u8 operand2;
    std::array<std::byte, padding> m_padding;

    [[nodiscard]] bool operator==(const UP_Instruction<padding>& rhs) const noexcept = default;

    [[nodiscard]] bool destination_is_immediate() const noexcept;


    [[nodiscard]] bool operand1_is_immediate() const noexcept;
    [[nodiscard]] bool operand2_is_immediate() const noexcept;
    [[nodiscard]] bool operand1_is_used() const noexcept;
    [[nodiscard]] bool operand2_is_used() const noexcept;

    [[nodiscard]] bool op1_is_reg() const noexcept;
    [[nodiscard]] bool op2_is_reg() const noexcept;


    const char* opcode_to_string() const noexcept;
};

template<>
struct UP_Instruction<0> {
    Opcode opcode;
    u8 destination;
    u8 operand1;
    u8 operand2;

    [[nodiscard]] bool operator==(const UP_Instruction<0>& rhs) const noexcept = default;

    [[nodiscard]] bool destination_is_immediate() const noexcept;


    [[nodiscard]] bool operand1_is_immediate() const noexcept;
    [[nodiscard]] bool operand2_is_immediate() const noexcept;
    [[nodiscard]] bool operand1_is_used() const noexcept;
    [[nodiscard]] bool operand2_is_used() const noexcept;

    [[nodiscard]] bool op1_is_reg() const noexcept;
    [[nodiscard]] bool op2_is_reg() const noexcept;


    const char* opcode_to_string() const noexcept;
};

template<u8 padding>
const char* UP_Instruction<padding>::opcode_to_string() const noexcept {
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

template<u8 padding>
[[nodiscard]] bool UP_Instruction<padding>::destination_is_immediate() const noexcept {
    return opcode == Opcode::Branch || opcode == Opcode::BranchIf
        || opcode == Opcode::BranchIfNot || opcode == Opcode::Return || opcode == Opcode::AssertPointer;
}
template<u8 padding>
[[nodiscard]] bool UP_Instruction<padding>::operand1_is_immediate() const noexcept {
    return opcode == Opcode::LoadStaticI32Imm || opcode == Opcode::LoadStaticI16Imm
        || opcode == Opcode::LoadStaticU16Imm || opcode == Opcode::LoadStaticI8Imm
        || opcode == Opcode::LoadStaticU8Imm || opcode == Opcode::LoadStaticU32Imm
        || opcode == Opcode::LoadStaticI64Imm || opcode == Opcode::LoadStaticU64Imm
        || opcode == Opcode::LoadStaticFloatImm || opcode == Opcode::LoadStaticPointer
        || opcode == Opcode::LoadStaticPointerImm
        || opcode == Opcode::LoadU16Imm || opcode == Opcode::LookupPointer;
}
template<u8 padding>
[[nodiscard]] bool UP_Instruction<padding>::operand2_is_immediate() const noexcept {
    return opcode == Opcode::Call || opcode == Opcode::CallFf
        || opcode == Opcode::IAddImm || opcode == Opcode::IMulImm
        || opcode == Opcode::ISubImm || opcode == Opcode::IDivImm
        || opcode == Opcode::LoadU16Imm;
}
template<u8 padding>
[[nodiscard]] bool UP_Instruction<padding>::operand1_is_used() const noexcept {
    return opcode != Opcode::Branch && opcode != Opcode::AssertPointer;
}
template<u8 padding>
[[nodiscard]] bool UP_Instruction<padding>::operand2_is_used() const noexcept {
    u32 num = static_cast<u32>(opcode);
    bool is_arithmetic = num >= static_cast<u32>(Opcode::IAdd) && num <= static_cast<u32>(Opcode::FDiv);
    bool is_call = opcode == Opcode::CallFf || opcode == Opcode::Call;
    bool is_comp = num >= static_cast<u32>(Opcode::IEqual) && num <= static_cast<u32>(Opcode::FMod);
    bool is_bit = opcode == Opcode::OpBitAnd || (num >= static_cast<u32>(Opcode::OpBitOr) && num <= static_cast<u32>(Opcode::OpLogOr));
    bool is_arithmetic_imm = num >= static_cast<u32>(Opcode::IAddImm) && num <= static_cast<u32>(Opcode::IDivImm);
    bool is_store = num >= static_cast<u32>(Opcode::StoreI8) && num <= static_cast<u32>(Opcode::FNotEqual) || num >= static_cast<u32>(Opcode::StoreInt) && num <= static_cast<u32>(Opcode::StorePointer);
    return is_arithmetic || is_call || is_comp || is_bit || is_arithmetic_imm || is_store;
}
template<u8 padding>
[[nodiscard]] bool UP_Instruction<padding>::op1_is_reg() const noexcept {
    return operand1_is_used() && !operand1_is_immediate() && operand1 < ARGUMENT_REGISTERS_IDX;
}
template<u8 padding>
[[nodiscard]] bool UP_Instruction<padding>::op2_is_reg() const noexcept {
    return operand2_is_used() && !operand2_is_immediate() && operand2 < ARGUMENT_REGISTERS_IDX;
}

using Instruction = UP_Instruction<4>;
using ShortInstruction = UP_Instruction<0>;

static_assert(sizeof(Instruction) == 8);
static_assert(sizeof(ShortInstruction) == 4);

struct function_disassembly_line {
    Instruction m_instruction;
    istr_line m_location;
    std::string m_text;
    const Instruction* m_globalPointer;
    std::string m_comment;
    u16 m_target = std::numeric_limits<u16>::max();
    bool m_isArgMove;

    function_disassembly_line() noexcept = default;

    function_disassembly_line(u64 idx, const Instruction* ptr) noexcept :
        m_instruction(ptr[idx]),
        m_location(idx),
        m_globalPointer(ptr),
        m_isArgMove(false)
    {}
};

struct Register {
    ast::full_type m_type;
    bool m_isReturn = false;
    bool m_containsArg = false;
    u8 m_argNum;
    u64 m_value = 0;
    u16 m_pointerOffset = std::numeric_limits<u16>::max();
    u8 m_fromSymbolTable = std::numeric_limits<u8>::max();

    static constexpr u64 UNKNOWN_VAL = std::numeric_limits<u64>::max() - 1;

    inline void set_first_type(const ast::full_type& type) noexcept {
        if (is_unknown(m_type)) {
            m_type = type;
        }
    }

    inline void set_first_type(const ast::primitive_kind& type) noexcept {
        if (is_unknown(m_type)) {
            m_type = make_type_from_prim(type);
        }
    }

    [[nodiscard]] inline bool is_pointer() const noexcept {
        return std::holds_alternative<ast::ptr_type>(m_type);
    }
};

struct SymbolTable {
    location m_location;
    std::vector<ast::full_type> m_types;

    template<typename T, bool is_64_bit = true>
    [[nodiscard]] const T& get(const u64 offset) const noexcept {
        return m_location.get<T>(offset * 8);
    }
};


struct StackFrame {
    std::array<Register, MAX_REGISTER> m_registers;
    SymbolTable m_symbolTable;
    std::vector<u32> m_labels;
    std::vector<function_disassembly_line> m_backwardsJumpLocs;
    std::vector<ast::full_type> m_registerArgs;
    ast::full_type m_returnType;

    StackFrame(location symbol_table = location(nullptr)) noexcept : m_registers{}, m_symbolTable{symbol_table, {}} {
        for (i32 i = ARGUMENT_REGISTERS_IDX; i < MAX_REGISTER; ++i) {
            m_registers[i].m_containsArg = true;
            m_registers[i].m_argNum = i - ARGUMENT_REGISTERS_IDX;
        }
    }

    Register& operator[](const u64 idx) noexcept;

    void to_string(char* buffer, const u64 buffer_size, const u64 idx, const char* resolved = "") const noexcept;

    void add_target_label(const u32 target) {
        auto res = std::find(m_labels.begin(), m_labels.end(), target);
        if (res == m_labels.end()) {
            m_labels.push_back(target);
        }
    }
};

struct ss_state {
    std::string m_name;
    u32 m_idx;
};

struct ss_track {
    std::string m_name;
    u32 m_idx;
};

struct ss_event {
    std::string m_name;
    u32 m_idx;
};

struct state_script_function_id {
    ss_state m_state;
    ss_track m_track;
    ss_event m_event;
    u64 m_idx;

    static constexpr std::string_view SEP = "@";

    [[nodiscard]] std::string to_string() const noexcept {
        const std::string idx_str = std::to_string(m_idx);
        const size_t total_size =
            m_state.m_name.size() + m_track.m_name.size() + m_event.m_name.size() + idx_str.size() + 3 * SEP.size();
        std::string result;
        result.reserve(total_size);
        result += m_state.m_name;
        result += SEP;
        result += m_track.m_name;
        result += SEP;
        result += m_event.m_name;
        result += SEP;
        result += idx_str;
        return result;
    }
};

using function_name_variant = std::variant<std::string, state_script_function_id>;

struct function_disassembly {
    std::vector<function_disassembly_line> m_lines;
    StackFrame m_stackFrame;
    function_name_variant m_id;
    u64 m_originalOffset;
    bool m_isScriptFunction;

    function_disassembly(std::vector<function_disassembly_line> lines, StackFrame stack_frame, function_name_variant id, bool is_script_function) noexcept :
    m_lines(std::move(lines)), m_stackFrame(std::move(stack_frame)), m_id(std::move(id)), m_isScriptFunction(is_script_function) {};
    
    [[nodiscard]] const std::string& get_id() const noexcept {
        if (!m_isScriptFunction) {
            return std::get<std::string>(m_id);
        } else {
            if (m_stateScriptId.empty()) {
                m_stateScriptId = std::get<state_script_function_id>(m_id).to_string();
            }
            return m_stateScriptId;
        }
    }

private:
    mutable std::string m_stateScriptId;
}; 


}
