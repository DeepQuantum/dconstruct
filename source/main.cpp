#include "mainwindow.h"

#include <QApplication>
#include "disassembly/listing_view_controller.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    //a.setStyleSheet("Fusion");
    MainWindow w;
    
    SIDBase *base = new SIDBase();
    base->load("C:/Users/damix/Documents/GitHub/TLOU2Modding/TLOU_DC_Tool_v1.01/sid1/sidbase_sorted.bin");
    w.m_sidbase = base;
    BinaryFile test("C:/Users/damix/Documents/GitHub/TLOU2Modding/tlou2_disasm/test/weapon-gameplay.bin");
    test.disassemble_file(*base);
    FILE *file = fopen("C:/Users/damix/Documents/GitHub/TLOU2Modding/tlou2_disasm/test.txt", "w");
    ListingViewController *controller = new ListingViewController(test, &w, base, file);
    w.show();
 
    delete base;
    return a.exec();
}
