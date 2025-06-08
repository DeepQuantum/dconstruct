#include "mainwindow.h"

#include <string>
#include "disassembly/base.h"
#include <fstream>
#include <filesystem>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    QPalette p = this->getListingView()->palette();
    p.setColor(QPalette::Base, MainWindow::BACKGROUND_COLOR);
    p.setColor(QPalette::Text, MainWindow::COMMENT_COLOR);
    getListingView()->setPalette(p);
}


