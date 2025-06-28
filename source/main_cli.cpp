#include "disassembly/file_disassembler.h"
#include "disassembly/edit_disassembler.h"
#include "cxxopts.hpp"
#include "about.h"
#include <chrono>
#include <iostream>
#include <filesystem>
#include <execution>

static void disasm_file(
    const std::filesystem::path &inpath, 
    const std::filesystem::path &out_filename, 
    const dconstruct::SIDBase &base,
    const dconstruct::DisassemblerOptions &options,
    const std::vector<std::string> &edits = {}) {
    
    const auto start = std::chrono::high_resolution_clock::now();
    //std::cout << "disassembling " << inpath << "... \n";
    dconstruct::BinaryFile file(inpath.string());
    if (!file.dc_setup()) {
        return;
    }

    if (!edits.empty()) {
        dconstruct::EditDisassembler ed(&file, &base, options, edits);
        ed.apply_file_edits();
    }

    std::filesystem::path output_location;
    if (out_filename.empty()) {
        auto cwd = std::filesystem::current_path();
        output_location = (cwd / inpath).concat(".txt");
    } else {
        output_location = out_filename;
    }
    //std::cout << output_location << '\n';
    dconstruct::FileDisassembler disassembler(&file, &base, output_location.string(), options);
    disassembler.disassemble();

    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
    
    //std::cout << "took " << time_taken.count() << "ms\n";
}

static void disassemble_multiple(
    const std::filesystem::path &in, 
    const std::filesystem::path &out, 
    const dconstruct::SIDBase &sidbase, 
    const dconstruct::DisassemblerOptions &options) {

    std::vector<std::filesystem::path> filepaths;
        
    for (const auto& entry : std::filesystem::recursive_directory_iterator(in)) {
        if (entry.path().extension() != ".bin") {
            continue;
        }
        std::filesystem::path output_file_path = out;
        //output_file_path /= entry.path().filename().concat(".txt");
        filepaths.emplace_back(entry.path());
        //disasm_file(entry.path().string(), output_file_path, sidbase, options);
    }

    const auto start = std::chrono::high_resolution_clock::now();

    std::cout << "disassembling " << filepaths.size() << " files...";

    std::for_each(
        std::execution::par_unseq,
        filepaths.begin(),
        filepaths.end(),
        [&](const std::filesystem::path &entry) {
            const std::filesystem::path outpath = (out / std::filesystem::relative(entry, in)).concat(".txt");
            std::filesystem::create_directories(outpath.parent_path());
            disasm_file(entry.string(), outpath, sidbase, options);
        }
    );

    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);


    std::cout << "took " << time_taken.count() << "ms\n";
}

int main(int argc, char *argv[]) {

    cxxopts::Options options("tlou2_disasm_cli", "\na program for disassembling and editing tlouii dc files. use --about for a more detailed description.\n");

    options.add_options()
        ("a,about", "print about")
        ("i,input",  "input DC file or folder", cxxopts::value<std::string>(), "<path>")
        ("o,output", "output file or folder", cxxopts::value<std::string>()->default_value(""), "<path>")
        ("s,sidbase", "sidbase file", cxxopts::value<std::string>()->default_value("sidbase.bin"), "<path>")
        ("e,edit", "make an edit at a specific address. may only be specified during single file disassembly.", cxxopts::value<std::vector<std::string>>(), "<address>@<variable>=<new_value>")
        ("indent", "number of spaces per indentation level in the output file", cxxopts::value<u8>()->default_value("2"), "n")
        ("eo,emit_once", "only emit the first occurence of a struct. repeating instances will still show the address but not the contents of the struct.", 
            cxxopts::value<b8>()->default_value("false"))
    ;

    options.parse_positional({"i"});

    auto result = options.parse(argc, argv);

    if (result.count("a") > 0) {
        print_about();
        return -1;
    }
    
    if (result.count("i") == 0) {
        std::cout << options.help() << '\n';
        return -1;
    }

    
    const std::filesystem::path filepath = result["i"].as<std::string>();
    const std::filesystem::path output = result["o"].as<std::string>();

    if (!std::filesystem::exists(filepath)) {
        std::cout << "error: input filepath " << filepath << " doesn't exist\n";
    }
    if (!std::filesystem::exists(output)) {
        std::cout << "error: output filepath " << filepath << " doesn't exist\n";
    }

    const b8 output_is_folder = std::filesystem::is_directory(output);

    const std::filesystem::path sidbase_path = result["s"].as<std::string>();

    const u8 indent_per_level = result["indent"].as<u8>();
    const b8 emit_once = result["eo"].as<b8>();
    std::vector<std::string> edits;
    if (result.count("e") > 0) {
        edits = result["e"].as<std::vector<std::string>>();
    }
    const dconstruct::DisassemblerOptions disassember_options {
        indent_per_level,
        emit_once,
    };

    dconstruct::SIDBase base{};
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
        disasm_file(filepath, output, base, disassember_options, edits);
    }
    return 0;
}