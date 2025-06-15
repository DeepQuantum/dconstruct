#include "disassembler.h"

class ListingViewDisassembler : public Disassembler {
    
    MainWindow *m_mainWindow;

    

    ListingViewDisassembler(BinaryFile *file, const SIDBase *sidbase, MainWindow *window) {
        this->m_currentFile = file;
        this->m_sidbase = sidbase;
        this->m_mainWindow = window;
        this->m_mainWindow->get_listing_view()->clear();
        this->m_mainWindow->get_listing_view()->setReadOnly(true);
    }
    
    void insert_span(const char *line, const TextFormat &text_format, const u64 indent) override {
        auto* view = this->m_mainWindow->get_listing_view();
        QTextCursor cursor = view->textCursor();
        cursor.movePosition(QTextCursor::End);
        view->setTextCursor(cursor);
        QTextCharFormat format;
        const QColor color = QColor(text_format.m_color.r, text_format.m_color.g, text_format.m_color.b); 
        format.setForeground(color);
        format.setFontPointSize(text_format.m_fontSize);
        if (indent != 0) {
            cursor.insertText(QString(indent, ' ') + QString::fromStdString(line), format);
        } else {
            cursor.insertText(QString::fromStdString(line), format);
        }
    }

    void complete() override {
        this->m_mainWindow->get_listing_view()->moveCursor(QTextCursor::Start);
        this->m_mainWindow->get_listing_view()->ensureCursorVisible();
    }
};