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

    
    

    QTextEdit *get_listing_view() const noexcept {
        return ui.ListingView;
    }

    QGraphicsView *get_graphics_view() const noexcept {
        return ui.graphicsView;
    }

    std::vector<BinaryFile> m_scripts;

private:
    Ui::MainWindow ui;
};
