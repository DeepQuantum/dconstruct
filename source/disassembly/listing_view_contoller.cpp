#include "listing_view_controller.h"
#include "../mainwindow.h"
#include <stdexcept>


ListingViewController::ListingViewController(BinaryFile &file, MainWindow *mainWindow) {
    this->m_mainWindow = mainWindow;
    this->m_currentFile = file;
    this->createListingView();
}

void ListingViewController::insertLine(const std::string &line, QColor color, int fontSize, u8 indent) {
    QTextCursor cursor = this->m_mainWindow->getListingView()->textCursor();
    cursor.movePosition(QTextCursor::End);
    this->m_mainWindow->getListingView()->setTextCursor(cursor);


    QTextCharFormat format;
    format.setForeground(color);
    format.setFontPointSize(fontSize);
    cursor.insertText(QString::fromStdString(std::string(indent, ' ') + line + "\n"), format);
}

void ListingViewController::createListingView() {
    this->insertHeaderLine();
}

void ListingViewController::insertHeaderLine() {
    std::string current_script_name = this->m_mainWindow->m_sidbase[this->m_currentFile.m_dcscript->m_stateScriptId];
    this->m_mainWindow->getListingView()->clear();
    this->m_mainWindow->getListingView()->setReadOnly(true);
    this->insertLine("DeepQuantum's DC Disassembler", MainWindow::STRING_COLOR, 14, 20);
    this->insertLine("Listing for script: " + current_script_name, MainWindow::STRING_COLOR, 14, 20);
    this->insertLine("Script ID: " + MainWindow::intToSIDString(this->m_currentFile.m_dcscript->m_stateScriptId), MainWindow::STRING_COLOR, 14, 20);
    this->insertLine("Filesize: " + std::to_string(this->m_currentFile.m_size) + " bytes", MainWindow::STRING_COLOR, 14, 20);
}