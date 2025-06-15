#include "mainwindow.h"

#include <string>
#include "disassembly/base.h"
#include <fstream>
#include <filesystem>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    this->ui.setupUi(this);
    QPalette p = this->get_listing_view()->palette();
    //p.setColor(QPalette::Base);
    //p.setColor(QPalette::Text);
    this->get_listing_view()->setPalette(p);
    this->get_graphics_view()->setRenderHint(QPainter::Antialiasing);  // Optional for better visuals
    this->get_graphics_view()->setWindowTitle("CFG Element Example");
    this->get_graphics_view()->resize(400, 300);
    this->get_graphics_view()->show();
}


