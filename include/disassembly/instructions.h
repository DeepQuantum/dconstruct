
#pragma once
#include "base.h"
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <array>

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

struct Instruction {
    Opcode opcode;
    u8 destination;
    u8 operand1;
    u8 operand2;
    u32 padding;

    [[nodiscard]] b8 operator==(const Instruction& rhs) const noexcept = default;

    [[nodiscard]] b8 destination_is_immediate() const noexcept;

    [[nodiscard]] b8 operand1_is_immediate() const noexcept;

    [[nodiscard]] b8 operand2_is_immediate() const noexcept;
    [[nodiscard]] b8 operand1_is_used() const noexcept;
    [[nodiscard]] b8 operand2_is_used() const noexcept;


    const char* opcode_to_string() const noexcept;
};

enum class SymbolTableEntryType {
    STRINGID_64,
    FLOAT,
    INT32,
    INT64,
    STRING,
    POINTER,
    FUNCTION,
    UNKNOWN_TYPE,
    NONE
};


struct SymbolTableEntry {
    SymbolTableEntryType m_type;
    union {
        sid64 m_hash;
        f32 m_f32;
        i32 m_i32;
        i64 m_i64;
        p64 m_pointer;
    };
};

struct function_disassembly_line {
    Instruction m_instruction;
    u64 m_location;
    std::string m_text;
    const Instruction* m_globalPointer;
    std::string m_comment;
    i64 m_target = -1;
    b8 m_isArgMove;

    function_disassembly_line() noexcept = default;

    function_disassembly_line(u64 idx, const Instruction* ptr) noexcept :
        m_instruction(ptr[idx]),
        m_location(idx),
        m_globalPointer(ptr),
        m_isArgMove(false)
    {}
};



struct RegisterPointer {
    p64 m_base;
    u64 m_offset;
    sid64 m_sid;

    RegisterPointer() noexcept : m_base(0), m_offset(0), m_sid(0) {};

    RegisterPointer(p64 base, u64 offset, sid64 sid) noexcept : m_base(base), m_offset(offset), m_sid(sid) {};

    p64 get() const noexcept {
        return m_base + m_offset;
    }
};

enum class RegisterValueType {
    I8,
    U8,
    BOOL,
    I16,
    U16,
    F16,
    I32,
    U32,
    F32,
    F64,
    I64,
    U64,
    HASH,
    POINTER,
    I8_POINTER,
    U8_POINTER,
    I16_POINTER,
    U16_POINTER,
    I32_POINTER,
    U32_POINTER,
    I64_POINTER,
    U64_POINTER,
    F32_POINTER,
    STRING,
    DARRAY,
    DDICT,
    UNKNOWN,
    VOID
};

struct Register {
    RegisterValueType m_type;
    union {
        b8                 m_BOOL;
        i8			       m_I8;
        u8			       m_U8;
        i16			       m_I16;
        u16		           m_U16;
        u16		           m_F16;
        i32			       m_I32;
        u32		           m_U32;
        f32		           m_F32;
        f64		           m_F64;
        int64_t		       m_I64;
        uint64_t	       m_U64;
        sid64              m_SID;
        RegisterPointer    m_PTR;
    };
    b8 isReturn = false;
    b8 isArg = false;
    u8 argNum;


    void set_first_type(RegisterValueType type) {
        if (m_type == RegisterValueType::UNKNOWN) {
            m_type = type;
        }
    }
};

struct StackFrame {
    std::array<Register, 128> m_registers;
    std::vector<SymbolTableEntry> m_symbolTableEntries;
    std::vector<u32> m_labels;
    std::vector<function_disassembly_line> m_backwardsJumpLocs;
    std::vector<RegisterValueType> m_registerArgs;
    location m_symbolTable;

    StackFrame(location symbol_table = location(nullptr)) noexcept : m_registers{}, m_symbolTable(symbol_table) {
        for (i32 i = 49; i < 70; ++i) {
            m_registers[i].isArg = true;
            m_registers[i].argNum = i - 49;
            m_registers[i].m_type = RegisterValueType::UNKNOWN;
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


struct function_disassembly {
    std::vector<function_disassembly_line> m_lines;
    StackFrame m_stackFrame;
    std::string m_id;

    inline void remove_redundant_check_branches() {
        for (auto& line : m_lines) {
            auto& istr = line.m_instruction;
            if ((istr.opcode == Opcode::BranchIf || istr.opcode == Opcode::BranchIfNot)) {
                const auto& target = m_lines[istr.destination].m_instruction;
                if (istr.opcode == target.opcode && istr.operand1 == target.operand1) {
                    istr.destination = target.destination;
                }
            }
        }
    }

}; 


}
