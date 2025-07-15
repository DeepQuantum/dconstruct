#pragma once

#include "base.h"
#include "binaryfile.h"
#include "instructions.h"
#include "custom_structs.h"
#include <vector>

namespace dconstruct {
    struct Color {
        u8 r, g, b;
    };

    constexpr static Color TYPE_COLOR = Color{ 78, 201, 176 };
    constexpr static Color VAR_COLOR = Color{ 156, 220, 254 };
    constexpr static Color CONTROL_COLOR = Color{ 197, 134, 192 };
    constexpr static Color NUM_COLOR = Color{ 181, 206, 163 };
    constexpr static Color OPCODE_COLOR = Color{ 84, 179, 183 };
    constexpr static Color STRING_COLOR = Color{ 174, 51, 68 };
    constexpr static Color FUNCTION_COLOR = Color{ 220, 220, 155 };
    constexpr static Color COMMENT_COLOR = Color{ 204, 204, 204 };
    constexpr static Color BACKGROUND_COLOR = Color{ 28, 29, 30 };
    constexpr static Color HASH_COLOR = Color{ 86, 156, 214 };

    struct TextFormat {
        Color m_color = COMMENT_COLOR;
        u64 m_fontSize = 14;
    };

    struct DisassemblerOptions {
        u8 m_indentPerLevel = 2;
        b8 m_emitOnce = false;
    };

    class Disassembler {
    public:
        void disassemble();
        u64 m_versionNumber = 0x1;

    protected:
        Disassembler() = default;
        virtual void insert_span(const char* text, const u32 indent = 0, const TextFormat& text_format = TextFormat{}) = 0;
        virtual ~Disassembler() = 0;

        BinaryFile* m_currentFile = nullptr;
        const SIDBase* m_sidbase = nullptr;
        DisassemblerOptions m_options;

        std::map<sid64, std::vector<const structs::unmapped*>> m_unmappedEntries;

        constexpr static TextFormat ENTRY_HEADER_FMT = { VAR_COLOR, 20 };
        constexpr static TextFormat ENTRY_TYPE_FMT = { TYPE_COLOR, 20 };
        constexpr static TextFormat STRUCT_TYPE_FMT = { TYPE_COLOR, 14 };
        constexpr static TextFormat COMMENT_FMT = { COMMENT_COLOR, 14 };

        FILE* m_perfFile = nullptr;

        void insert_entry(const Entry* entry);
        void insert_struct(const structs::unmapped* entry, const u32 indent = 0, const sid64 name_id = 0);
        template<TextFormat text_format = TextFormat{}, typename... Args> 
        void insert_span_fmt(const char* format, Args ...args);
        template<TextFormat text_format = TextFormat{}, typename... Args> 
        void insert_span_indent(const char* format, const u32 indent, Args ...args);
        [[nodiscard]] const char* lookup(const sid64 hash) noexcept;
        [[nodiscard]] b8 is_unmapped_sid(const location) const noexcept;
        void insert_header_line();
        [[nodiscard]] b8 is_possible_float(const f32* ptr) const noexcept;
        [[nodiscard]] b8 is_possible_i32(const i32* ptr) const noexcept;
        u8 insert_struct_or_arraylike(const location, const u32) noexcept;
        [[nodiscard]] u32 get_size_array(const location, const u32) noexcept;
        void insert_anonymous_array(const location, const u32) noexcept;
        void insert_array(const location, const u32, const u32);
        void insert_state_script(const StateScript* stateScript, const u32 indent);
        void insert_unmapped_struct(const structs::unmapped* _struct, const u32 indent);
        u8 insert_next_struct_member(const location, const u32);
        void insert_variable(const SsDeclaration* var, const u32);
        void insert_on_block(const SsOnBlock* block, const u32);
        [[nodiscard]] FunctionDisassembly create_function_disassembly(const ScriptLambda* lambda, const sid64 name_id = 0);
        void process_instruction(StackFrame& stackFrame, FunctionDisassemblyLine& functionLine);
        void insert_function_disassembly_text(const FunctionDisassembly& functionDisassembly, const u32 indent);
        void insert_label(const std::vector<u32>& labels, const FunctionDisassemblyLine& line, const u32 func_size, const u32 indent) noexcept;
        void insert_goto_label(const std::vector<u32>& labels, const FunctionDisassemblyLine& line, const u32 func_size, const std::vector<FunctionDisassemblyLine>& lines) noexcept;
        [[nodiscard]] u32 get_offset(const location) const noexcept;
        [[nodiscard]] u32 get_offset(const void*) const noexcept;
    };
}
