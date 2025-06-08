#pragma once

#include "base.h"
#include "binaryfile.h"
#include <QtWidgets/QTextEdit>
#include "../mainwindow.h"
#include "instructions.h"

struct TextFormat {
    QColor m_color = MainWindow::COMMENT_COLOR;
    u64 m_fontSize = 14;
};

struct DC_Struct {
    stringid_64 m_typeID;
    const void *m_data;
};

class ListingViewController {
public:
    ListingViewController(BinaryFile &file, MainWindow *mainWindow, const SIDBase *sidbase, FILE *outfptr = nullptr);
    ~ListingViewController();
    void create_listing_view();
    void insert_entry(const Entry *entry);
    void insert_struct(const DC_Struct *entry, const u64 indent = 0);
    void insert_span(const char *text, const TextFormat &text_format = TextFormat{}, const u64 indent = 0);
    template<typename... Args> void insert_span_fmt(const char *format, const TextFormat &text_format = TextFormat{}, const u64 indent = 0, Args ...args);

private:
    BinaryFile m_currentFile;
    MainWindow *m_mainWindow;
    const SIDBase *m_sidbase;
    FILE *m_outfptr;

    std::map<stringid_64, std::vector<const DC_Struct*>> m_unmappedEntries;

    constexpr static TextFormat ENTRY_HEADER_FMT = {.m_color = MainWindow::VAR_COLOR, .m_fontSize = 20};
    constexpr static TextFormat ENTRY_TYPE_FMT   = {.m_color = MainWindow::TYPE_COLOR, .m_fontSize = 20};
    constexpr static TextFormat STRUCT_TYPE_FMT  = {.m_color = MainWindow::TYPE_COLOR, .m_fontSize = 14};

    [[nodiscard]] const char *lookup(const stringid_64 hash) noexcept;
    void insert_header_line();
    b8 is_possible_float(const f32 *ptr);
    void disassemble_state_script(const StateScript *stateScript);
    void insert_unmapped_struct(const DC_Struct *_struct, const u64 indent);
    void insert_variable(const SsDeclaration *var);
    void insert_on_block(const SsOnBlock *block);
    FunctionDisassembly create_function_disassembly(const ScriptLambda *lambda);
    void process_instruction(StackFrame &stackFrame, FunctionDisassemblyLine &functionLine);
    void insert_function_disassembly_text(const FunctionDisassembly &functionDisassembly);
    u32 get_offset(const void *symbol);
};