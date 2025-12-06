
#pragma once
#include "base.h"
#include "ast/type.h"
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

    [[nodiscard]] bool operator==(const Instruction& rhs) const noexcept = default;

    [[nodiscard]] bool destination_is_immediate() const noexcept;


    [[nodiscard]] bool operand1_is_immediate() const noexcept;
    [[nodiscard]] bool operand2_is_immediate() const noexcept;
    [[nodiscard]] bool operand1_is_used() const noexcept;
    [[nodiscard]] bool operand2_is_used() const noexcept;

    [[nodiscard]] bool op1_is_reg() const noexcept;
    [[nodiscard]] bool op2_is_reg() const noexcept;


    const char* opcode_to_string() const noexcept;
};

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

struct Register {
    ast::full_type m_type;
    bool m_isReturn = false;
    bool m_containsArg = false;
    u8 m_argNum;
    u64 m_value = 0;
    u16 m_pointerOffset = UINT16_MAX;
    u8 m_fromSymbolTable = UINT8_MAX;

    inline void set_first_type(const ast::full_type& type) noexcept {
        if (is_unknown(m_type)) {
            m_type = type;
        }
    }

    inline void set_first_type(const ast::primitive_kind& type) noexcept {
        if (is_unknown(m_type)) {
            m_type = make_type(type);
        }
    }

    [[nodiscard]] inline bool is_pointer() const noexcept {
        return std::holds_alternative<ast::ptr_type>(m_type);
    }
};

using SymbolTable = std::pair<location, std::vector<ast::full_type>>;

struct StackFrame {
    std::array<Register, 128> m_registers;
    SymbolTable m_symbolTable;
    std::vector<u32> m_labels;
    std::vector<function_disassembly_line> m_backwardsJumpLocs;
    std::vector<ast::full_type> m_registerArgs;
    ast::full_type m_returnType;

    StackFrame(location symbol_table = location(nullptr)) noexcept : m_registers{}, m_symbolTable{symbol_table, {}} {
        for (i32 i = 49; i < 70; ++i) {
            m_registers[i].m_containsArg = true;
            m_registers[i].m_argNum = i - 49;
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
    bool m_isScriptFunction;
}; 


}
