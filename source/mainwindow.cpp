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
    p.setColor(QPalette::Text, MainWindow::BLANK_COLOR);
    getListingView()->setPalette(p);
}

void MainWindow::load_sidbase(const std::string &path) noexcept {
    u64 num_entries, loc, hash_value;
    std::ifstream sidfile(path, std::ios::binary);
    std::size_t fsize = std::filesystem::file_size(path);

    if (!sidfile.is_open()) {
        exit(-1);
    }

    u8 *m_sidbytes = new u8[fsize];

    sidfile.read(reinterpret_cast<char*>(&num_entries), 8);
    sidfile.read(reinterpret_cast<char*>(m_sidbytes), fsize);

    this->m_sidbase = SIDBase{
        num_entries,
        m_sidbytes - 8,
        reinterpret_cast<SIDBaseEntry*>(m_sidbytes),
    };
}

MainWindow::~MainWindow() {
    delete[] this->m_sidbytes;
}



[[nodiscard]] const std::string MainWindow::lookup(const stringid_64 sid) const noexcept {
    const char *hash = this->m_sidbase.search(sid, this->m_sidbase.m_entries, this->m_sidbase.m_num_entries);
    if (hash == nullptr) {
        return int_to_string_id(sid);
    } else {
        return hash;
    }
}

const std::string MainWindow::int_to_string_id(stringid_64 sid) {
    char buffer[16] = {0};
    sprintf(buffer, "#%016llX", sid);
    return buffer;
}

const std::string MainWindow::offset_to_string(u32 offset) {
    char buffer[8];
    sprintf(buffer, "0x%06llX", offset);
    return buffer;
}
