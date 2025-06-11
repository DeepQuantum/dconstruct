#include "disassembly/listing_view_controller.h"
#include <chrono>


int main(int argc, char *argv[]) {

    auto start = std::chrono::high_resolution_clock::now();

    if (argc != 2) {
        std::cout << "Usage: tlou_disasm_cli <dc_file.bin>" << "\n";
        exit(1);
    }

    const char* filepath = argv[1];

    std::cout << "Disassembling " << filepath << "\n";

    SIDBase base{};
    base.load("C:/Users/damix/Documents/GitHub/TLOU2Modding/TLOU_DC_Tool_v1.01/sid1/sidbase_sorted.bin");
    BinaryFile test(filepath);
    test.disassemble_file(base);
    FILE *file = fopen("C:/Users/damix/Documents/GitHub/TLOU2Modding/tlou2_disasm/test.txt", "w");
    
    ListingViewController controller(&test, nullptr, &base, file);
    controller.create_listing_view();
    
    auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);

    std::cout << "\n\nTotal: " << time_taken.count() << " ms";

    return 0;
}