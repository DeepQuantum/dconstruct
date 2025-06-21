#include "disassembly/file_disassembler.h"
#include "cxxopts.hpp"
#include <chrono>
#include <iostream>
#include <filesystem>

void disasm_file(const char *inpath, const char *out_filename, const SIDBase &base, const DisassemblerOptions &options) {
    const auto start = std::chrono::high_resolution_clock::now();

    std::cout << "disassembling " << inpath << "... ";

    BinaryFile file(inpath);
    file.dc_setup();
    if (out_filename == nullptr) {
        auto cwd = std::filesystem::current_path();
        
    }
    const std::string out_file_path = "C:/Users/damix/Documents/GitHub/TLOU2Modding/tlou2_disasm/output/" + std::string(out_filename) + ".txt";
    FileDisassembler disassembler(&file, &base, out_file_path, options);
    disassembler.disassemble();

    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
    
    std::cout << "took " << time_taken.count() << "ms\n";
}

void disassemble_multiple(const char *in, const char *out, const SIDBase &sidbase, const DisassemblerOptions &options) {
    for (const auto& entry : std::filesystem::directory_iterator(in)) {
        if (entry.path().extension() != ".bin") {
            continue;
        }
        disasm_file(entry.path().string().c_str(), entry.path().filename().string().c_str(), sidbase, options);
    }
}

int main(int argc, char *argv[]) {

    cxxopts::Options options("tlou2_disasm_cli", "a program for disassembling and editing tlouii dc files.");

    options.add_options()
        ("i,input",  "input DC file or folder", cxxopts::value<std::string>(), "<path>")
        ("o,output", "output file or folder", cxxopts::value<std::string>(), "<path>")
        ("s,sidbase", "sidbase file", cxxopts::value<std::string>()->default_value("sidbase.bin"), "<path>")
        ("e,edit", "make an edit at a specific address. may only be specified during single file disassembly.", cxxopts::value<std::string>(), "<address>@<variable>=<new_value>")
        ("indent", "number of spaces per indentation level", cxxopts::value<u8>()->default_value("2"), "n")
        ("eo,emit_once", "emit structs only once, regardless if they appear in multiple locations. the repeating instance will be replaced by a reference to the first emittance.", 
            cxxopts::value<b8>()->default_value("false")->implicit_value("true"))
    ;
    //-e E98@sid1=test
    auto result = options.parse(argc, argv);
    
    if (result.count("i") == 0) {
        std::cout << options.help() << '\n';
        return -1;
    }
    
    const char *filepath = result["i"].as<const char*>();
    const char *output = result["o"].as<const char*>();

    const b8 output_is_folder = std::filesystem::is_directory(output);

    const char *sidbase_path = result["s"].as<const char*>();

    const u8 indent_per_level = result["indent"].as<u8>();
    const b8 emit_once = result["eo"].as<b8>();
    
    const DisassemblerOptions disassember_options {
        indent_per_level,
        emit_once,
    };

    SIDBase base{};
    base.load(sidbase_path);

    if (std::filesystem::is_directory(filepath)) {
        if (!output_is_folder) {
            std::cout << "the input " << filepath << " is a folder, but output " << output << " is a file." << '\n';
            return -1;
        }
        disassemble_multiple(filepath, output, base, disassember_options);
    } else {
        if (output_is_folder) {
            std::cout << "the input " << filepath << " is a file, but output " << output << " is a folder." << '\n';
        }
        disasm_file(filepath, output, base, disassember_options);
    }
    return 0;
}