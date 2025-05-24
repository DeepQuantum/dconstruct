#include "mainwindow.h"

#include <iostream>

#include <QApplication>
#include "disassembly/disassembler.h"
#include "disassembly/listing_view_controller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet("Fusion");
    MainWindow w;
    
    Disassembler *dis = new Disassembler();
    dis->loadSidbase("C:/Users/damix/Documents/GitHub/TLOU2Modding/TLOU_DC_Tool_v1.01/sid1/sidbase.bin");
    BinaryFile test("C:/Users/damix/Documents/GitHub/TLOU2Modding/tlou2_disasm/test/ss-ellie-hoodie-manager.bin");
    dis->disassembleFile(test);
    ListingViewController *controller = new ListingViewController(dis, w.getListingView());
    w.show();

    return a.exec();
}
