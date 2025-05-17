#include "mainwindow.h"

#include <QApplication>
#include "disassembler.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet("Fusion");
    MainWindow w;

    Disassembler *dis = new Disassembler();
    dis->loadSidbase("C:/Users/damix/Documents/GitHub/TLOU2Modding/TLOU_DC_Tool_v1.01/sid1/sidbase.bin");
    BinaryFile test("C:/Users/damix/Documents/GitHub/TLOU2Modding/tlou2_disasm/ss-ellie-hoodie-manager.bin");
    dis->disassembleFile(test);

    w.show();
    return a.exec();
}
