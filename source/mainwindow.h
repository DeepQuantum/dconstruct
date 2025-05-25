#pragma once

#include <QMainWindow>
#include "ui_mainwindow.h"

#include "disassembly/base.h"
#include "disassembly/binaryfile.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <execution>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    constexpr static QColor OPCODE_COLOR = QColor(84, 179, 183);
    constexpr static QColor STRING_COLOR = QColor(159, 41, 68);
    constexpr static QColor COMMENT_COLOR = QColor(255, 255, 0);
    constexpr static QColor BLANK_COLOR = QColor(183, 210, 210);
    constexpr static QColor NUM_LITERAL_COLOR = QColor(112, 160, 112);
    constexpr static QColor BACKGROUND_COLOR = QColor(28, 29, 30);
    
    std::unordered_map<stringid_64, std::string> m_sidbase;
    constexpr static u8 VersionNumber = 0x01;


    QTextEdit *getListingView() const noexcept {
        return ui.ListingView;
    }

    void loadSidbase(const std::string &path = "sidbase.bin");
    std::string resolveHash(stringid_64 sid) const;
    std::vector<BinaryFile> m_scripts;
    static std::string intToSIDString(stringid_64 sid);
    static std::string offsetToString(u32 offset);

private:
    Ui::MainWindow ui;
};