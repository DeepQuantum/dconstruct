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

    void insertLine(const std::string &line, QColor color = MainWindow::BLANK_COLOR, int fontSize = 10, u8 indent = 0);
    void insertHeaderLine();

    void appendLine(const std::string &line);
};