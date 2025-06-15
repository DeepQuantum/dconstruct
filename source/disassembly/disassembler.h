#pragma once

#include "base.h"
#include "binaryfile.h"
#include "instructions.h"
#include <vector>

struct Color {
    u8 r, g, b;
};

constexpr static Color TYPE_COLOR = Color{78, 201, 176};
constexpr static Color VAR_COLOR = Color{156, 220, 254};
constexpr static Color CONTROL_COLOR = Color{197, 134, 192};
constexpr static Color NUM_COLOR = Color{181, 206, 163};
constexpr static Color OPCODE_COLOR = Color{84, 179, 183};
constexpr static Color STRING_COLOR = Color{174, 51, 68};
constexpr static Color FUNCTION_COLOR = Color{220, 220, 155};
constexpr static Color COMMENT_COLOR = Color{204, 204, 204};
constexpr static Color BACKGROUND_COLOR = Color{28, 29, 30};
constexpr static Color HASH_COLOR = Color{86, 156, 214};

struct TextFormat {
    Color m_color = COMMENT_COLOR;
    u64 m_fontSize = 14;
};

struct DC_Struct {
    sid64 m_typeID;
    const void *m_data;
};

class Disassembler {
public:
    void disassemble();
    u64 m_versionNumber = 0x1;

protected:
    Disassembler() = default;
    virtual void insert_span(const char *text, const TextFormat &text_format = TextFormat{}, const u64 indent = 0) = 0;
    virtual void complete() = 0;

    BinaryFile *m_currentFile = nullptr;
    const SIDBase *m_sidbase = nullptr;

    std::map<sid64, std::vector<const DC_Struct*>> m_unmappedEntries;

    constexpr static TextFormat ENTRY_HEADER_FMT = {VAR_COLOR, 20};
    constexpr static TextFormat ENTRY_TYPE_FMT   = {TYPE_COLOR, 20};
    constexpr static TextFormat STRUCT_TYPE_FMT  = {TYPE_COLOR, 14};
    constexpr static TextFormat COMMENT_FMT      = {COMMENT_COLOR, 14};

    constexpr static u32 m_indentPerLevel = 2;
    
    void insert_entry(const Entry *entry);
    void insert_struct(const DC_Struct *entry, const u64 indent = 0);
    template<typename... Args> void insert_span_fmt(const char *format, const TextFormat &text_format = TextFormat{}, Args ...args);
    [[nodiscard]] const char *lookup(const sid64 hash) noexcept;
    void insert_header_line();
    [[nodiscard]] b8 is_possible_float(const f32 *ptr) const noexcept;
    void insert_state_script(const StateScript *stateScript, const u32 indent);
    void insert_unmapped_struct(const DC_Struct *_struct, const u64 indent);
    void insert_variable(const SsDeclaration *var, const u32 indent);
    void insert_on_block(const SsOnBlock *block, const u32 indent);
    FunctionDisassembly create_function_disassembly(const ScriptLambda *lambda);
    void process_instruction(StackFrame &stackFrame, FunctionDisassemblyLine &functionLine);
    void insert_function_disassembly_text(const FunctionDisassembly &functionDisassembly, const u32 indent);
    void insert_label(const std::vector<u32> &labels, const FunctionDisassemblyLine &line, const u32 func_size, const u32 indent) noexcept;
    void insert_goto_label(const std::vector<u32> &labels, const FunctionDisassemblyLine &line, const u32 func_size, const std::vector<FunctionDisassemblyLine> &lines) noexcept;
    [[nodiscard]] u32 get_offset(const void *symbol) const noexcept;
};