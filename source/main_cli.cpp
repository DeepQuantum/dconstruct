#include "disassembly/file_disassembler.h"
#include "cxxopts.hpp"
#include <chrono>
#include <iostream>
#include <filesystem>
#include <execution>

static void disasm_file(const std::filesystem::path &inpath, const std::filesystem::path &out_filename, const SIDBase &base, const DisassemblerOptions &options) {
    const auto start = std::chrono::high_resolution_clock::now();
    std::cout << "disassembling " << inpath << "... ";
    BinaryFile file(inpath.string());
    if (!file.dc_setup()) {
        return;
    }
    std::string output;
    if (out_filename.empty()) {
        auto cwd = std::filesystem::current_path();
        const std::string infile = inpath.filename().string();
        output = cwd.concat("\\" + infile + ".txt").string();
    } else {
        output = out_filename.string();
    }
    FileDisassembler disassembler(&file, &base, output, options);
    disassembler.disassemble();

    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
    
    std::cout << "took " << time_taken.count() << "ms\n";
}

static void disassemble_multiple(const std::filesystem::path &in, const std::filesystem::path &out, const SIDBase &sidbase, const DisassemblerOptions &options) {
    std::vector<std::filesystem::path> filepaths;
        
    for (const auto& entry : std::filesystem::recursive_directory_iterator(in)) {
        if (entry.path().extension() != ".bin") {
            continue;
        }
        std::filesystem::path output_file_path = out;
        output_file_path.concat(entry.path().filename().string() + ".txt");
        filepaths.emplace_back(entry.path());
        //disasm_file(entry.path().string(), output_file_path, sidbase, options);
    }

    std::for_each(
        std::execution::seq,
        filepaths.begin(),
        filepaths.end(),
        [&](const std::filesystem::path &entry) {
            std::filesystem::path output_file_path = out;
            output_file_path.concat(std::filesystem::relative(entry, in).string() + ".txt");
            disasm_file(entry.string(), output_file_path, sidbase, options);
        }
    );
}

int main(int argc, char *argv[]) {

    cxxopts::Options options("tlou2_disasm_cli", "a program for disassembling and editing tlouii dc files.");

    options.add_options()
        ("i,input",  "input DC file or folder", cxxopts::value<std::string>(), "<path>")
        ("o,output", "output file or folder", cxxopts::value<std::string>()->default_value(""), "<path>")
        ("s,sidbase", "sidbase file", cxxopts::value<std::string>()->default_value("sidbase.bin"), "<path>")
        ("e,edit", "make an edit at a specific address. may only be specified during single file disassembly.", cxxopts::value<std::string>(), "<address>@<variable>=<new_value>")
        ("indent", "number of spaces per indentation level", cxxopts::value<u8>()->default_value("2"), "n")
        ("eo,emit_once", "emit structs only once, regardless if they appear in multiple locations. the repeating instance will be replaced by a reference to the first emittance.", 
            cxxopts::value<b8>()->default_value("false")->implicit_value("true"))
    ;
    //-e E98@var_1=test,
    auto result = options.parse(argc, argv);
    
    if (result.count("i") == 0) {
        std::cout << options.help() << '\n';
        return -1;
    }
    
    const std::filesystem::path filepath = result["i"].as<std::string>();
    const std::filesystem::path output = result["o"].as<std::string>();

    const b8 output_is_folder = std::filesystem::is_directory(output);

    const std::filesystem::path sidbase_path = result["s"].as<std::string>();

    const u8 indent_per_level = result["indent"].as<u8>();
    const b8 emit_once = result["eo"].as<b8>();
    std::string edits = "";
    if (result.count("e") > 0) {
        edits = result["e"].as<std::string>();
    }
    const DisassemblerOptions disassember_options {
        indent_per_level,
        emit_once,
    };

    SIDBase base{};
    base.load(sidbase_path);

    if (std::filesystem::is_directory(filepath)) {
        if (!output_is_folder) {
            std::cout << "error: the input " << filepath << " is a folder, but output " << output << " is a file." << '\n';
            return -1;
        }
        if (!edits.empty()) {
            std::cout << "warning: edits ignored as input/output path is a directory. edits only work in single file disassembly." << '\n';
        }
        disassemble_multiple(filepath, output, base, disassember_options);
    } else {
        if (output_is_folder) {
            std::cout << "error: the input " << filepath << " is a file, but output " << output << " is a folder." << '\n';
            return -1;
        }
        disasm_file(filepath, output, base, disassember_options);
    }
    return 0;
}