#include "base.h"
#include "binaryfile.h"
#include <QtWidgets/QTextEdit>
#include "../mainwindow.h"
#include "instructions.h"


class ListingViewController {
public:
    ListingViewController(BinaryFile &file, MainWindow *mainWindow);
    void setScriptById(stringid_64 id);
    void createListingView();
    void insertSpan(const std::string &text, QColor color = MainWindow::BLANK_COLOR, int fonzSize = 10, u8 indent = 0);

private:
    BinaryFile m_currentFile;
    MainWindow *m_mainWindow;

    void insertComment(const std::string &comment, u8 indent = 0);
    void insertHash(const std::string &hash, b8 surround = false);
    void insertHeaderLine();
    void disassembleStateScript(StateScript *stateScript);
    void parseCustomStruct(const Symbol &symbol);
    void insertVariable(SsDeclaration *var);
    void insertOnBlock(SsOnBlock *block);
    FunctionDisassembly createFunctionDisassembly(ScriptLambda *lambda);
    void processInstruction(StackFrame &stackFrame, FunctionDisassemblyLine &functionLine);
    void insertFunctionDisassemblyText(const FunctionDisassembly &functionDisassembly);
    u32 getOffset(const void *symbol);

    void parseMap(const Symbol &symbol) noexcept;
};