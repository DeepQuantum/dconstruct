#pragma once

#include "base.h"
#include "binaryfile.h"
#include "instructions.h"
#include "custom_structs.h"
#include <vector>
#include <unordered_map>

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

    struct anonymous_function_name {
        std::string m_state;
        std::string m_track;
        std::string m_event;
        std::string m_idx;

        static constexpr std::string_view SEP = "@";

        [[nodiscard]] std::string get() const noexcept {
            const size_t total_size =
                m_state.size() + m_track.size() + m_event.size() + m_idx.size() +
                3 * SEP.size();

            std::string result;
            result.reserve(total_size);
            result += m_state;
            result += SEP;
            result += m_track;
            result += SEP;
            result += m_event;
            result += SEP;
            result += m_idx;
            return result;
        }
    };

    class Disassembler {
    public:
        Disassembler(BinaryFile* file, const SIDBase* sidbase) noexcept : m_currentFile(file), m_sidbase(sidbase) {}

        void disassemble();
        u64 m_versionNumber = 0x1;
        virtual ~Disassembler() {};
        [[nodiscard]] function_disassembly create_function_disassembly(const ScriptLambda* lambda, const std::string& name, const b8 is_script_function = false);
        [[nodiscard]] function_disassembly create_function_disassembly(std::vector<Instruction>&&, const std::string&, const location& symbol_table, const b8 is_script_function = false);

        [[nodiscard]] const std::vector<function_disassembly>& get_functions() const noexcept {
            return m_functions;
        }

    protected:
        virtual void insert_span(const char* text, const u32 indent = 0, const TextFormat& text_format = TextFormat{}) {};
        
        BinaryFile* m_currentFile = nullptr;
        const SIDBase* m_sidbase = nullptr;
        DisassemblerOptions m_options;
        std::vector<function_disassembly> m_functions;

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
        void insert_span_indent(const char*, const u32, Args ...);
        [[nodiscard]] const char* lookup(const sid64 hash);
        [[nodiscard]] b8 is_unmapped_sid(const location) const noexcept;
        void insert_header_line();
        [[nodiscard]] b8 is_possible_float(const f32* ptr) const noexcept;
        [[nodiscard]] b8 is_possible_i32(const i32* ptr) const noexcept;
        u8 insert_struct_or_arraylike(const location, const u32);
        [[nodiscard]] u32 get_size_array(const location, const u32);
        void insert_anonymous_array(const location, const u32);
        void insert_array(const location, const u32, const u32);
        void insert_state_script(const StateScript*, const u32);
        void insert_unmapped_struct(const structs::unmapped*, const u32);
        u8 insert_next_struct_member(const location, const u32);
        void insert_variable(const SsDeclaration* var, const u32);
        void insert_on_block(const SsOnBlock* block, const u32, anonymous_function_name& state_name);
        void set_register_types(Register&, Register&, const ast::full_type type);
        void process_instruction(const u32, function_disassembly &);
        void insert_function_disassembly_text(const function_disassembly& functionDisassembly, const u32 indent);
        void insert_label(const std::vector<u32>& labels, const function_disassembly_line& line, const u32 func_size, const u32 indent);
        void insert_goto_label(const std::vector<u32>& labels, const function_disassembly_line& line, const u32 func_size, const std::vector<function_disassembly_line>& lines);
        [[nodiscard]] u32 get_offset(const location) const noexcept;
        [[nodiscard]] u32 get_offset(const void*) const noexcept;
        [[nodiscard]] b8 pointer_gets_called(const u32, const u32, const function_disassembly&) const;
        b8 handle_immediate_instruction(const Instruction& istr, StackFrame& frame, function_disassembly_line& line,const function_disassembly& fn,char* disassembly_text,char* varying, const u32 disassembly_text_size, char* interpreted) const noexcept;

        static constexpr u32 INTERPRETED_BUFFER_SIZE = 512;
        static constexpr u32 DISASSEMBLY_BUFFER_SIZE = 256;
    };

    const static std::unordered_map<sid64, ast::function_type> builtinFunctions = {
        {SID("get-int32"), ast::make_function(make_type(ast::primitive_kind::I32), {
            {"symbol", make_type(ast::primitive_kind::SID)},
            {"location", make_type(ast::primitive_kind::SID)}
        })},
        {SID("get-boolean"), ast::make_function(make_type(ast::primitive_kind::BOOL), {
            {"symbol", make_type(ast::primitive_kind::SID)},
            {"location", make_type(ast::primitive_kind::SID)}
        })}
    };
}
