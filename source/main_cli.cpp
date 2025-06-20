#include "disassembly/file_disassembler.h"
#include "cxxopts.hpp"
#include <chrono>
#include <iostream>
#include <filesystem>

void disasm_file(const char *inpath, const SIDBase *base, const char *out_filename) {
    BinaryFile file(inpath);
    file.dc_setup();
    printf("Disassembling %s\n", out_filename);
    const std::string out_file_path = "C:/Users/damix/Documents/GitHub/TLOU2Modding/tlou2_disasm/output/" + std::string(out_filename) + ".txt";
    FileDisassembler disassembler(&file, base, out_file_path);
    disassembler.disassemble();
}

int main(int argc, char *argv[]) {

    cxxopts::Options options("DC Disaassembler");

    options.add_options()
        ("i,input", "input DC file", cxxopts::value<std::string>())
    ;
    
    auto result = options.parse(argc, argv);
    
    if (result.count("i") == 0) {
        std::cout << options.help() << std::endl;
        exit(0);
    }
    const char *filepath = result["i"].as<const char*>();

    std::cout << "disassembling " << filepath << "\n";
    
    SIDBase base{};
    base.load("C:/Users/damix/Documents/GitHub/TLOU2Modding/TLOU_DC_Tool_v1.01/sid1/sidbase_sorted.bin");
    
    auto start = std::chrono::high_resolution_clock::now();

    const std::string base_path = "C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1";

    disasm_file(filepath, &base, "test.txt");

    // for (const auto& entry : std::filesystem::directory_iterator(base_path)) {
    //     if (entry.path().extension() != ".bin" || entry.path().filename().string()[0] != 'w') {
    //         continue;
    //     }
    //     disasm_file(entry.path().string().c_str(), &base, entry.path().filename().string().c_str());
    // }
    
    auto time_taken = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);

    std::cout << "\n\nTotal: " << time_taken.count() << " us";
    return 0;
}