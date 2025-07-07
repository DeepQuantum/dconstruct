#include "disassembly/file_disassembler.h"
#include "disassembly/edit_disassembler.h"
#include "cxxopts.hpp"
#include "about.h"
#include <chrono>
#include <iostream>
#include <filesystem>
#include <execution>

static constexpr char DEFAULT_OUT[] = "<input_path.txt>";

static void disasm_file(
    const std::filesystem::path &inpath, 
    const std::filesystem::path &out_filename, 
    const dconstruct::SIDBase &base,
    const dconstruct::DisassemblerOptions &options,
    const std::vector<std::string> &edits = {}) {
    
    dconstruct::BinaryFile file(inpath.string());
    if (!file.dc_setup()) {
        return;
    }

    if (!edits.empty()) {
        dconstruct::EditDisassembler ed(&file, &base, options, edits);
        ed.apply_file_edits();
    }

    dconstruct::FileDisassembler disassembler(&file, &base, out_filename.string(), options);
    disassembler.disassemble();
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
        filepaths.emplace_back(entry.path());
    }

    const auto start = std::chrono::high_resolution_clock::now();

    std::cout << "disassembling " << filepaths.size() << " files into " << out << "...\n";

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

static std::vector<std::string> edits_from_file(const std::filesystem::path &path) {
    std::ifstream edit_in(path);
    std::vector<std::string> result;

    if (!edit_in.is_open()) {
        std::cout << "warning: couldn't open " << path << '\n'; 
    }
    std::string edit_str;           
    while (edit_in >> edit_str) {
        result.emplace_back(std::move(edit_str));
    }

    return result;
}

int main(int argc, char *argv[]) {

    cxxopts::Options options("dconstruct", "\na program for disassembling and editing tlouii dc files. use --about for a more detailed description.\n");

    options.add_options("information")
        ("h, help", "display this message")
        ("help_edit", "help with editing a file")
        ("a,about", "print about");
    options.add_options("input/output")
        ("i,input",  "input DC file or folder", cxxopts::value<std::string>(), "<path>")
        ("o,output", "output file or folder", cxxopts::value<std::string>()->default_value(""), DEFAULT_OUT)
        ("s,sidbase", "sidbase file", cxxopts::value<std::string>()->default_value("sidbase.bin"), "<path>");
    options.add_options("configuration")
        ("indent", "number of spaces per indentation level in the output file", cxxopts::value<u8>()->default_value("2"), "n")
        ("emit_once", "only emit the first occurence of a struct. repeating instances will still show the address but not the contents of the struct.", 
            cxxopts::value<b8>()->default_value("false"));
    options.add_options("edit")
        ("e,edit", "make an edit at a specific address. may only be specified during single file disassembly.", cxxopts::value<std::vector<std::string>>(), "<addr>[<offset>]=<new_value>")
        ("edit_file", "specify a path to an edit file. a line in an edit file is equivalent to the value for one -e flag.", cxxopts::value<std::string>())
    ;

    options.parse_positional({"i"});

    auto opts = options.parse(argc, argv);

    if (opts.count("h") > 0) {
        std::cout << options.help({"", "information", "input/output", "configuration", "edit"}) << '\n';
        return -1;
    }

    if (opts.count("help_edit") > 0) {
        std::cout << HELP_EDIT;
        return -1;
    }

    if (opts.count("a") > 0) {
        print_about();
        return -1;
    }
    
    std::filesystem::path filepath;
    if (opts.count("i") == 0) {
        std::cout << "error: no input specified\n";
        return -1;
    } else {
        filepath = opts["i"].as<std::string>();
        if (!std::filesystem::exists(filepath)) {
            std::cout << "error: input filepath " << filepath << " doesn't exist\n";
            return -1;
        }
    }

    std::vector<std::string> edits{};
    if (opts.count("edit_file") > 0) {
        std::string test = opts["edit_file"].as<std::string>();
        edits = edits_from_file(test);
    }

    std::filesystem::path output;
    if (opts.count("o") == 0) {
        if (!std::filesystem::is_directory(filepath)) {
            output = filepath.string() + ".txt";
        } else {
            constexpr char default_out_folder_path[] = "./disassembled";
            std::filesystem::create_directory(default_out_folder_path);
            output = default_out_folder_path;
        }
    } else {
        output = opts["o"].as<std::string>();
        if (!std::filesystem::exists(output)) {
            std::cout << "error: output filepath " << output << " doesn't exist\n";
            return -1;
        }
        if (std::filesystem::is_directory(output) && !std::filesystem::is_directory(filepath)) {
            output /= filepath.filename().string() + ".txt";
        }
    }

    const b8 output_is_folder = std::filesystem::is_directory(output);

    const std::filesystem::path sidbase_path = opts["s"].as<std::string>();
    if (!std::filesystem::exists(sidbase_path)) {
        std::cout << "error: sidbase path " << sidbase_path << " doesn't exist\n";
        return -1;
    }

    const u8 indent_per_level = opts["indent"].as<u8>();
    const b8 emit_once = opts["emit_once"].as<b8>();
    if (opts.count("e") > 0) {
        std::vector<std::string> edit_strings = opts["e"].as<std::vector<std::string>>();
        edits.insert(edits.end(), edit_strings.begin(), edit_strings.end());
    }
    const dconstruct::DisassemblerOptions disassember_options {
        indent_per_level,
        emit_once,
    };

    dconstruct::SIDBase base{};
    base.load(sidbase_path);

    if (std::filesystem::is_directory(filepath)) {
        if (!output_is_folder) {
            std::cout << "error: the input " << filepath << " is a folder, but output " << output << " is a file.\n";
            return -1;
        }
        if (!edits.empty()) {
            std::cout << "warning: edits ignored as input path is a directory. edits only work in single file disassembly.\n";
        }
        disassemble_multiple(filepath, output, base, disassember_options);
    } else {
        std::cout << "disassembling " << filepath.filename() << " to " << output << "...\n";
        const auto start = std::chrono::high_resolution_clock::now();
        disasm_file(filepath, output, base, disassember_options, edits);
        const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
        std::cout << "took " << time_taken.count() << "ms\n";
    }
    return 0;
}