#include "disassembly/file_disassembler.h"
#include "disassembly/edit_disassembler.h"
#include "decompilation/decomp_function.h"
#include "shaders/ndshader.h"
#include "cxxopts.hpp"
#include "about.h"
#include "windows.h"
#include <locale>
#include <codecvt>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <execution>

static constexpr char DEFAULT_OUT[] = "<input_path.asm>";

[[nodiscard]] static std::filesystem::path get_sanitized_graph_path(const std::filesystem::path& graph_dir, const std::string &func_id) {
    std::string sanitized_func_id;
    sanitized_func_id.reserve(func_id.size());
    for (char c : func_id) {
        switch (c) {
            case '?':
            case '>':
            case '<':
            case '*':
            case '\\':
            case '/':
            case '|':
            case '\"':
            case ':':
            case '@':
            case '-': {
                sanitized_func_id += '_';
                break;
            }
            default: {
                sanitized_func_id += c;
            }
        }
    }
    return (graph_dir / sanitized_func_id).replace_extension(".svg");
} 

static void decomp_file(
    const std::filesystem::path &inpath, 
    const std::filesystem::path &out_disasm_filename, 
    const std::filesystem::path &out_decomp_filename,
    const dconstruct::SIDBase &base,
    const dconstruct::DisassemblerOptions &options,
    const bool write_graphs,
    const dconstruct::ast::print_fn_type language_type,
    const std::vector<std::string> &edits = {}) {
    
    auto file_res = dconstruct::BinaryFile<true>::from_path(inpath.string());

    if (!file_res) {
        std::cerr << file_res.error() << "\n";
        std::terminate();
    }

    //std::cout << inpath << std::endl;

    auto& file = *file_res;

    if (!edits.empty()) {
        dconstruct::EditDisassembler ed(&file, &base, options, edits);
        ed.apply_file_edits();
    }

    dconstruct::FileDisassembler disassembler(&file, &base, out_disasm_filename.string(), options);
    disassembler.disassemble();
    disassembler.dump();
    const auto funcs = disassembler.get_named_functions();
    if (!funcs.empty()) {
        std::ofstream out(out_decomp_filename);
        std::vector<dconstruct::ast::function_definition> functions;
        functions.reserve(funcs.size());
        out << language_type;
        for (const auto& func : funcs) {
            std::optional<std::filesystem::path> graph_path = std::nullopt;
            if (write_graphs) {
                auto graph_dir = (std::filesystem::path(out_decomp_filename).replace_extension("").concat("_graphs"));
                std::filesystem::create_directories(graph_dir);
                graph_path = get_sanitized_graph_path(graph_dir, func->get_id());
            }
            try {
                functions.emplace_back(dconstruct::dcompiler::decomp_function{ *func, file, dconstruct::ControlFlowGraph::build(*func), std::move(graph_path) }.decompile(true));
            }
            catch (const std::exception& e) {
                std::cout << "warning: couldn't decompile <" << func->get_id() << ">: " << e.what() << "\n";
            }
        }
        dconstruct::dcompiler::state_script_functions<> output_functions{functions};
        out << output_functions.to_string();
    }
}

static void disasm_file(
    const std::filesystem::path &inpath, 
    const std::filesystem::path &out_filename, 
    const dconstruct::SIDBase &base,
    const dconstruct::DisassemblerOptions &options,
    const std::vector<std::string> &edits = {}) {
    
    auto file_res = dconstruct::BinaryFile<true>::from_path(inpath.string());

    if (!file_res) {
        std::cerr << file_res.error() << "\n";
        std::terminate();
    }

    auto& file = *file_res;

    if (!edits.empty()) {
        dconstruct::EditDisassembler ed(&file, &base, options, edits);
        ed.apply_file_edits();
    }


    dconstruct::FileDisassembler disassembler(&file, &base, out_filename.string(), options);
    disassembler.disassemble();
    disassembler.dump();
}

static void decompile_multiple(
    const std::filesystem::path &in, 
    const std::filesystem::path &out, 
    const dconstruct::SIDBase &sidbase, 
    const dconstruct::DisassemblerOptions &options,
    const bool generate_graphs,
    const dconstruct::ast::print_fn_type language_print
) {

    std::vector<std::filesystem::path> filepaths;
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(in)) {
        if (entry.path().extension() != ".bin") {
            continue;
        }
        filepaths.emplace_back(entry.path());
    }

    const auto start = std::chrono::high_resolution_clock::now();

    std::cout << "disassembling & decompiling " << filepaths.size() << " files into " << out << "...\n";

    std::for_each(
        std::execution::par_unseq,
        filepaths.begin(),
        filepaths.end(),
        [&](const std::filesystem::path &entry) {
            const std::filesystem::path disasm_outpath = (out / std::filesystem::relative(entry, in)).concat(".asm");
            const std::filesystem::path decomp_outpath = (out / std::filesystem::relative(entry, in)).concat(".dcpl");
            std::filesystem::create_directories(disasm_outpath.parent_path());
            decomp_file(entry.string(), disasm_outpath, decomp_outpath, sidbase, options, generate_graphs, language_print, {});
        }
    );

    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);


    std::cout << "took " << time_taken.count() << "ms\n";
}

static void disassemble_multiple(
    const std::filesystem::path &in, 
    const std::filesystem::path &out, 
    const dconstruct::SIDBase &sidbase, 
    const dconstruct::DisassemblerOptions &options
) {

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
            const std::filesystem::path outpath = (out / std::filesystem::relative(entry, in)).concat(".asm");
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

static i32 disassemble_shader(const std::filesystem::path& path) {
    const auto ir_exp = dconstruct::shaders::ndshader_file::parse_from_file(path);
    if (!ir_exp) {
        std::cerr << "couldn't process shader file: " << ir_exp.error() << "\n";
        return -1;
    }
    const auto [msg, success] = ir_exp->to_string();
    if (!success) {
        std::cerr << msg << "\n";
        return -1;
    }
    std::cout << msg << "\n";
    return 0;
}

std::wstring get_executable_path()
{
    wchar_t buffer[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    if (len == 0 || len == MAX_PATH)
        throw std::runtime_error("GetModuleFileNameW failed");
    return std::wstring(buffer, len);
}

[[nodiscard]] static std::optional<dconstruct::ast::print_fn_type> get_print_type(const std::string& input_string) {
    if (input_string == "C" || input_string == "c") {
        return dconstruct::ast::c;
    } else if (input_string == "Python" || input_string == "python") {
        return dconstruct::ast::py;
    } else if (input_string == "Racket" || input_string == "racket") {
        return dconstruct::ast::racket;
    } else {
        return std::nullopt;
    }
}

int main(int argc, char *argv[]) {

    cxxopts::Options options("dconstruct", "\na program for disassembling, editing and decompiling tlouii dc files. use --about for a more detailed description.\n");

    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    std::filesystem::path current_program_path = converter.to_bytes(get_executable_path());

    options.add_options("information")
        ("h, help", "display this message")
        ("help_edit", "help with editing a file")
        ("a,about", "print about");
    options.add_options("input/output")
        ("i,input",  "input DC file or folder", cxxopts::value<std::string>(), "<path>")
        ("o,output", "output file or folder", cxxopts::value<std::string>()->default_value(""), DEFAULT_OUT)
        ("s,sidbase", "sidbase file", cxxopts::value<std::string>()->default_value((current_program_path.parent_path() / "sidbase.bin").string()), "<path>");
    options.add_options("configuration")
        ("decompile", "will also emit a file containing the decompiled (named) functions in the file", cxxopts::value<bool>()->default_value("false"))
        ("indent", "number of spaces per indentation level in the output file", cxxopts::value<u8>()->default_value("2"), "n")
        ("language", "specify the DCPL pseudo language type. current options are 'C', 'Racket' (closest to original DC), or 'Python'. default is 'C'.", cxxopts::value<std::string>()->default_value("C"))
        ("shader", "treat the input as a shader file instead.", cxxopts::value<bool>()->default_value("false"))
        ("graphs", "emit control flow graph SVGs of the named functions when decompiling. only emits graphs of size >1. SIGNIFICANTLY slows down decompilation..", cxxopts::value<bool>()->default_value("false"))
        ("emit_once", "only emit the first occurence of a struct. repeating instances will still show the address but not the contents of the struct.", 
            cxxopts::value<bool>()->default_value("false"))
        ("uc4", "experimental: try to disassemble/decompile an uncharted 4 .bin file instead", cxxopts::value<bool>()->default_value("false"));

    options.add_options("edit")
        ("e,edit", "make an edit at a specific address. may only be specified during single file disassembly.", cxxopts::value<std::vector<std::string>>(), "<addr>[<offset>]=<new_value>")
        ("edit_file", "specify a path to an edit file. a line in an edit file is equivalent to the value for one -e flag.", cxxopts::value<std::string>())
    ;

    options.parse_positional({"i"});
    cxxopts::ParseResult opts;
    try {
        opts = options.parse(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return -1;
    }

    if (opts.count("h") > 0) {
        std::cout << options.help({"", "information", "input/output", "configuration", "edit"}) << '\n';
        return 0;
    }

    if (opts.count("help_edit") > 0) {
        std::cout << HELP_EDIT;
        return 0;
    }

    if (opts.count("a") > 0) {
        print_about();
        return 0;
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

    if (opts.count("shader") > 0) {
       return disassemble_shader(filepath);
    }

    std::vector<std::string> edits{};
    if (opts.count("edit_file") > 0) {
        std::string test = opts["edit_file"].as<std::string>();
        edits = edits_from_file(test);
    }

    std::filesystem::path output;
    if (opts.count("o") == 0) {
        if (!std::filesystem::is_directory(filepath)) {
            output = filepath.string() + ".asm";
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
            output /= filepath.filename().string() + ".asm";
        }
    }

    const bool output_is_folder = std::filesystem::is_directory(output);

    const std::filesystem::path sidbase_path = opts["s"].as<std::string>();

    if (!std::filesystem::exists(sidbase_path)) {
        std::cout << "error: sidbase path " << sidbase_path << " doesn't exist\n";
        return -1;
    }

    const u8 indent_per_level = opts["indent"].as<u8>();
    const bool emit_once = opts["emit_once"].as<bool>();
    const bool decompile = opts["decompile"].as<bool>();
    const bool generate_graphs = opts["graphs"].as<bool>();
    const std::string language_type = opts["language"].as<std::string>();

    const auto opt_print_func = get_print_type(language_type);
    if (!opt_print_func) {
        std::cerr << "error: unknown language type: '" << language_type << "'\n";
        return -1;
    }
    const auto& print_func = *opt_print_func;
    

    if (opts.count("e") > 0) {
        std::vector<std::string> edit_strings = opts["e"].as<std::vector<std::string>>();
        edits.insert(edits.end(), edit_strings.begin(), edit_strings.end());
    }
    const dconstruct::DisassemblerOptions disassember_options {
        indent_per_level,
        emit_once,
    };

    auto base_exp = dconstruct::SIDBase::from_binary(sidbase_path);
    if (!base_exp) {
        std::cerr << base_exp.error();
        return -1;
    }
    const auto& base = *base_exp;

    if (std::filesystem::is_directory(filepath)) {
        if (!output_is_folder) {
            std::cout << "error: the input " << filepath << " is a folder, but output " << output << " is a file.\n";
            return -1;
        }
        if (!edits.empty()) {
            std::cout << "warning: edits ignored as input path is a directory. edits only work in single file disassembly.\n";
        }
        if (decompile) {
            if (generate_graphs) {
                std::filesystem::create_directory(output / "graphs");
            }
            decompile_multiple(filepath, output, base, disassember_options, generate_graphs, print_func);
        }
        else {
            disassemble_multiple(filepath, output, base, disassember_options);
        }
    } else {
        const auto start = std::chrono::high_resolution_clock::now();
        if (decompile) {
            std::cout << "disassembling & decompiling " << filepath.filename() << "...\n";
            decomp_file(filepath, output, std::filesystem::path(output).replace_extension(".dcpl"), base, disassember_options, generate_graphs, print_func, edits);
        }
        else {
            std::cout << "disassembling " << filepath.filename() << "...\n";
            disasm_file(filepath, output, base, disassember_options, edits);
        }
        const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
        std::cout << "took " << time_taken.count() << "ms\n";
    }
    return 0;
}