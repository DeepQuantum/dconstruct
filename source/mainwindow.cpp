#include "mainwindow.h"

#include <string>
#include "disassembly/base.h"
#include <fstream>
#include <filesystem>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    QPalette p = get_listing_view()->palette();
    //p.setColor(QPalette::Base);
    //p.setColor(QPalette::Text);
    get_listing_view()->setPalette(p);
    get_graphics_view()->setRenderHint(QPainter::Antialiasing);  // Optional for better visuals
    get_graphics_view()->setWindowTitle("CFG Element Example");
    get_graphics_view()->resize(400, 300);
    get_graphics_view()->show();
}


