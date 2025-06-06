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

void MainWindow::load_sidbase(const std::string &path) noexcept {
    u64 num_entries, loc, hash_value;
    std::ifstream sidfile(path, std::ios::binary);
    std::size_t fsize = std::filesystem::file_size(path);

    if (!sidfile.is_open()) {
        exit(-1);
    }

    u8 *sidbytes = new u8[fsize];

    sidfile.read(reinterpret_cast<char*>(&num_entries), 8);
    sidfile.seekg(0);
    sidfile.read(reinterpret_cast<char*>(sidbytes), fsize);

    this->m_sidbase.m_num_entries = num_entries;
    this->m_sidbase.m_sidbytes = sidbytes;
    this->m_sidbase.m_entries = reinterpret_cast<SIDBaseEntry*>(sidbytes + 8);
}


