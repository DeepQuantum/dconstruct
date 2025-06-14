#pragma once

#include <QMainWindow>
#include "ui_mainwindow.h"

#include "disassembly/base.h"
#include "disassembly/binaryfile.h"
#include "disassembly/sidbase.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    constexpr static QColor TYPE_COLOR = QColor(78, 201, 176);
    constexpr static QColor VAR_COLOR = QColor(156, 220, 254);
    constexpr static QColor CONTROL_COLOR = QColor(197, 134, 192);
    constexpr static QColor NUM_COLOR = QColor(181, 206, 163);
    constexpr static QColor OPCODE_COLOR = QColor(84, 179, 183);
    constexpr static QColor STRING_COLOR = QColor(174, 51, 68);
    constexpr static QColor FUNCTION_COLOR = QColor(220, 220, 155);

    constexpr static QColor COMMENT_COLOR = QColor(204, 204, 204);
    constexpr static QColor BACKGROUND_COLOR = QColor(28, 29, 30);
    constexpr static QColor HASH_COLOR = QColor(86, 156, 214);
    
    constexpr static u8 VersionNumber = 0x01;

    QTextEdit *get_listing_view() const noexcept {
        return this->ui.ListingView;
    }

    QGraphicsView *get_graphics_view() const noexcept {
        return this->ui.graphicsView;
    }

    std::vector<BinaryFile> m_scripts;

private:
    Ui::MainWindow ui;
};
