#include "mainwindow.h"

#include <string>
#include <unordered_map>
#include "disassembly/base.h"
#include <fstream>
#include <filesystem>
#include <intrin.h> //bittest

#include "disassembly/binaryfile.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    QPalette p = this->getListingView()->palette();
    p.setColor(QPalette::Base, MainWindow::BACKGROUND_COLOR);
    p.setColor(QPalette::Text, MainWindow::BLANK_COLOR);
    getListingView()->setPalette(p);
}

void MainWindow::loadSidbase(const std::string &path) {
    u64 num_entries, loc, hash_value;
    std::ifstream sidfile(path, std::ios::binary);
    std::size_t fsize = std::filesystem::file_size(path);

    if (!sidfile.is_open()) {
        exit(-1);
    }

    sidfile.read(reinterpret_cast<char*>(&num_entries), 8);

    char *bytes = new char[fsize];

    sidfile.read(bytes, fsize);

    for (std::size_t i = 0; i < num_entries; ++i) {
        hash_value = *reinterpret_cast<stringid_64*>(bytes + i * 16 + 0);
        loc = *reinterpret_cast<u64*>(bytes + i * 16 + 8);

        this->m_sidbase.emplace(hash_value, std::string(bytes + loc - 8));
    }
    delete[] bytes;
}

std::string MainWindow::resolveHash(stringid_64 sid) const {
    auto it = this->m_sidbase.find(sid);
    if (it != this->m_sidbase.end()) {
        return it->second;
    }
    return this->intToSIDString(sid);
}

std::string MainWindow::intToSIDString(stringid_64 sid) {
    char buffer[16];
    sprintf(buffer, "%016llX", sid);
    return std::string(buffer);
}

std::string MainWindow::offsetToString(u32 offset) {
    char buffer[8];
    sprintf(buffer, "0x%06llX", offset);
    return std::string(buffer);
}