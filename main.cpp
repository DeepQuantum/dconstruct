#include "mainwindow.h"

#include <QApplication>
#include "disassembler.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet("Fusion");
    MainWindow w;

    Disassembler *dis = new Disassembler();
    dis->loadSidbase("C:/Users/damix/Documents/GitHub/TLOU2Modding/t2r-dc-disasm/sid1/sidbase.bin");

    w.show();
    return a.exec();
}
