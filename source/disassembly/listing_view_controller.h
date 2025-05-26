#include "base.h"
#include "binaryfile.h"
#include <QtWidgets/QTextEdit>
#include "../mainwindow.h"


class ListingViewController {
public:
    ListingViewController(BinaryFile &file, MainWindow *mainWindow);
    void setScriptById(stringid_64 id);
    void createListingView();

private:
    BinaryFile m_currentFile;
    MainWindow *m_mainWindow;

    void insertSpan(const std::string &text, QColor color = MainWindow::BLANK_COLOR, int fonzSize = 10, u8 indent = 0);
    void insertComment(const std::string &comment, u8 indent = 0);
    void insertHash(const std::string &hash, b8 surround = false);
    void insertInstructioN(const std::string &instruction);
    void insertHeaderLine();
    void insertFunctionDisassembly(ScriptLambda *lambda);
    void insertSymbolTable(u64 *symbolPtr, const u8 size) const noexcept;
    u32 getOffset(const void *symbol);
};