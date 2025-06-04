#pragma once

#include "base.h"
#include "binaryfile.h"
#include <QtWidgets/QTextEdit>
#include "../mainwindow.h"
#include "instructions.h"


class ListingViewController {
public:
    ListingViewController(BinaryFile &file, MainWindow *mainWindow);
    void create_listing_view();
    void insert_entry(Entry *entry);
    void insert_span(const char *text, QColor color = MainWindow::BLANK_COLOR, int fonzSize = 10, u8 indent = 0);

private:
    BinaryFile m_currentFile;
    MainWindow *m_mainWindow;

    void insert_comment(const std::string &comment, u8 indent = 0);
    void insert_hash(const std::string &hash, b8 surround = false);
    [[nodiscard]] const char *lookup(const stringid_64 hash) noexcept;
    void insert_header_line();
    void disassemble_state_script(StateScript *stateScript);
    void parse_custom_struct(Entry *symbol);
    void parse_custom_strucht_from_pointer(const u64 *p);
    void insert_variable(SsDeclaration *var);
    void insert_on_block(SsOnBlock *block);
    FunctionDisassembly create_function_disassembly(ScriptLambda *lambda);
    void process_instruction(StackFrame &stackFrame, FunctionDisassemblyLine &functionLine);
    void insert_function_disassembly_text(const FunctionDisassembly &functionDisassembly);
    u32 get_offset(const void *symbol);
};