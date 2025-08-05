#include "disassembler.h"

class ListingViewDisassembler : public Disassembler {
    
    MainWindow *m_mainWindow;

    

    ListingViewDisassembler(BinaryFile *file, const SIDBase *sidbase, MainWindow *window) {
        m_currentFile = file;
        m_sidbase = sidbase;
        m_mainWindow = window;
        m_mainWindow->get_listing_view()->clear();
        m_mainWindow->get_listing_view()->setReadOnly(true);
    }
    
    void insert_span(const char *line, const TextFormat &text_format, const u64 indent) override {
        auto* view = m_mainWindow->get_listing_view();
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
        m_mainWindow->get_listing_view()->moveCursor(QTextCursor::Start);
        m_mainWindow->get_listing_view()->ensureCursorVisible();
    }
};