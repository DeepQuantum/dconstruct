#include "mainwindow.h"

#include <iostream>

#include <QApplication>
#include "disassembly/listing_view_controller.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    //a.setStyleSheet("Fusion");
    MainWindow w;
    
    w.loadSidbase("C:/Users/damix/Documents/GitHub/TLOU2Modding/TLOU_DC_Tool_v1.01/sid1/sidbase.bin");
    BinaryFile test("C:/Users/damix/Documents/GitHub/TLOU2Modding/tlou2_disasm/test/ai-action-packs.bin");
    test.disassembly_file(w.m_sidbase);
    ListingViewController *controller = new ListingViewController(test, &w);
    w.show();

    return a.exec();
}
