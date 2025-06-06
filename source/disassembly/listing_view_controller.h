#pragma once

#include "base.h"
#include "binaryfile.h"
#include <QtWidgets/QTextEdit>
#include "../mainwindow.h"
#include "instructions.h"

struct TextFormat {
    QColor m_color = MainWindow::COMMENT_COLOR;
    u64 m_fontSize = 12;
    u64 indent = 0;
};

class ListingViewController {
public:
    ListingViewController(BinaryFile &file, MainWindow *mainWindow);
    void create_listing_view();
    void insert_entry(const Entry *entry);
    void insert_span(const char *text, const TextFormat &text_format = TextFormat{});
    template<typename... Args> void insert_span_fmt(const char *format, const TextFormat &text_format = TextFormat{}, Args ...args);

private:
    BinaryFile m_currentFile;
    MainWindow *m_mainWindow;

    [[nodiscard]] const char *lookup(const stringid_64 hash) noexcept;
    void insert_header_line();
    void disassemble_state_script(const StateScript *stateScript);
    void parse_custom_struct(const Entry *symbol);
    void parse_custom_strucht_from_pointer(const u64 *p);
    void insert_variable(const SsDeclaration *var);
    void insert_on_block(const SsOnBlock *block);
    FunctionDisassembly create_function_disassembly(const ScriptLambda *lambda);
    void process_instruction(StackFrame &stackFrame, FunctionDisassemblyLine &functionLine);
    void insert_function_disassembly_text(const FunctionDisassembly &functionDisassembly);
    u32 get_offset(const void *symbol);
};