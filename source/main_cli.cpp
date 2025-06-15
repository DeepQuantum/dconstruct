#include "disassembly/file_disassembler.h"
#include <chrono>
#include <iostream>


int main(int argc, char *argv[]) {



    if (argc != 2) {
        std::cout << "Usage: tlou_disasm_cli <dc_file.bin>" << "\n";
        exit(1);
    }

    const char* filepath = argv[1];

    std::cout << "Disassembling " << filepath << "\n";

    SIDBase base{};
    base.load("C:/Users/damix/Documents/GitHub/TLOU2Modding/TLOU_DC_Tool_v1.01/sid1/sidbase_sorted.bin");
    BinaryFile test(filepath);
    auto start = std::chrono::high_resolution_clock::now();

    test.disassemble_file();
    const std::string out_file_path = "C:/Users/damix/Documents/GitHub/TLOU2Modding/tlou2_disasm/test.txt";
    //FILE *file2 = fopen("C:/Users/damix/Documents/GitHub/TLOU2Modding/tlou2_disasm/test2.txt", "w");
    
    FileDisassembler controller(&test, &base, out_file_path);
    //FileDisassembler controller2(&test, &base, file2);
    controller.disassemble();
    //controller2.disassemble();
    
    auto time_taken = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);

    std::cout << "\n\nTotal: " << time_taken.count() << " us";
    return 0;
}