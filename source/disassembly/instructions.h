
#pragma once
#include "base.h"
#include <string>
#include <vector>
#include <algorithm>
#include <map>

enum Opcode : u8 {
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
    const char *opcode_to_string() const noexcept;
    b8 isSymbolLoadInstruction() const noexcept;
    b8 isBranchInstruction() const noexcept;
};

enum SymbolTableEntryType { 
    STRINGID_64,
    FLOAT,
    INT,
    STRING,
    POINTER,
    UNKNOWN_TYPE,
    NONE
};

struct SymbolTableEntry {
    SymbolTableEntryType m_type;
    union {
        sid64 m_hash;
        f32 m_f32;
        i64 m_i64;
        p64 m_pointer;
    };
};

struct FunctionDisassemblyLine {
    Instruction m_instruction;
    u64 m_location;
    std::string m_text;
    Instruction *m_globalPointer;
    std::string m_comment;
    b8 m_isArgMove;
    i64 m_label = -1;
    std::vector<u64> m_locationsPointedFrom;

    FunctionDisassemblyLine() = default;

    FunctionDisassemblyLine(u64 idx, Instruction *ptr) :
        m_instruction(ptr[idx]),
        m_location(idx),
        m_globalPointer(ptr),
        m_isArgMove(false)
    {}
};



struct RegisterPointer {
    p64 m_base;
    u64 m_offset;

    p64 get() const noexcept {
        return m_base + m_offset;
    }
};

enum RegisterValueType {
    R_I8,
	R_U8,
    R_BOOL,
	R_I16,
	R_U16,
	R_F16,
	R_I32,
	R_U32,
	R_F32,
	R_F64,
    R_I64,
    R_U64,
    R_HASH,
    R_POINTER,
    R_STRING
};

struct Register {
    RegisterValueType m_type;
    union {
        i8			       m_I8;
        i8			       m_U8;
        b8                 m_BOOL;
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
};

struct StackFrame {
    Register m_registers[128];
    u64 *m_symbolTablePtr = nullptr;
    std::map<u32, SymbolTableEntry> m_symbolTable;
    std::vector<u32> m_labels;
    u32 m_argCount = 0;

    StackFrame() : m_registers{}, m_symbolTable{}, m_labels{} {
        for (i32 i = 49; i < 70; ++i) {
            m_registers[i].isArg = true;
            m_registers[i].argNum = i - 49;
        }
    }

    Register& operator[](const u64 idx) noexcept;

    void to_string(char *buffer, const u64 buffer_size, const u64 idx, const char *resolved = "") const noexcept;

    void add_target_label(const u32 target) noexcept {
        auto res = std::find(m_labels.begin(), m_labels.end(), target);
        if (res == m_labels.end()) {
            m_labels.push_back(target);
        }
    }
};


struct FunctionDisassembly {
    std::vector<FunctionDisassemblyLine> m_lines;
    StackFrame m_stackFrame;
};