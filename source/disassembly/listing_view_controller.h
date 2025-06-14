#pragma once

#include "base.h"
#include "binaryfile.h"
#include <QtWidgets/QTextEdit>
#include "../mainwindow.h"
#include "instructions.h"
#include <vector>

struct TextFormat {
    QColor m_color = MainWindow::COMMENT_COLOR;
    u64 m_fontSize = 14;
};

struct DC_Struct {
    sid64 m_typeID;
    const void *m_data;
};

class ListingViewController {
public:
    ListingViewController(BinaryFile *file, MainWindow *mainWindow, const SIDBase *sidbase, FILE *outfptr = nullptr);
    void create_listing_view();
    void insert_entry(const Entry *entry);
    void insert_struct(const DC_Struct *entry, const u64 indent = 0);
    void insert_span(const char *text, const TextFormat &text_format = TextFormat{}, const u64 indent = 0);
    template<typename... Args> void insert_span_fmt(const char *format, const TextFormat &text_format = TextFormat{}, Args ...args);

private:
    BinaryFile *m_currentFile = nullptr;
    MainWindow *m_mainWindow = nullptr;
    const SIDBase *m_sidbase = nullptr;
    FILE *m_outfptr = nullptr;
    std::string m_outbuf;

    std::map<sid64, std::vector<const DC_Struct*>> m_unmappedEntries;

    constexpr static TextFormat ENTRY_HEADER_FMT = {.m_color = MainWindow::VAR_COLOR, .m_fontSize = 20};
    constexpr static TextFormat ENTRY_TYPE_FMT   = {.m_color = MainWindow::TYPE_COLOR, .m_fontSize = 20};
    constexpr static TextFormat STRUCT_TYPE_FMT  = {.m_color = MainWindow::TYPE_COLOR, .m_fontSize = 14};

    constexpr static u32 m_indentPerLevel = 2;

    [[nodiscard]] const char *lookup(const sid64 hash) noexcept;
    void insert_header_line();
    b8 is_possible_float(const f32 *ptr);
    void insert_state_script(const StateScript *stateScript, const u32 indent);
    void insert_unmapped_struct(const DC_Struct *_struct, const u64 indent);
    void insert_variable(const SsDeclaration *var, const u32 indent);
    void insert_on_block(const SsOnBlock *block, const u32 indent);
    FunctionDisassembly create_function_disassembly(const ScriptLambda *lambda);
    void process_instruction(StackFrame &stackFrame, FunctionDisassemblyLine &functionLine);
    void insert_function_disassembly_text(const FunctionDisassembly &functionDisassembly, const u32 indent);
    u32 get_offset(const void *symbol);
};