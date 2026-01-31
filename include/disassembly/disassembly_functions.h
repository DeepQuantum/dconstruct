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

namespace dconstruct::disassembly {

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
    const bool show_warnings,
    const bool optimize,
    const std::vector<std::string> &edits = {}, 
    const bool use_pascal_case = false,
    const bool is_64_bit = true) {
    
    auto file_res = dconstruct::BinaryFile::from_path(inpath.string());

    if (!file_res) {
        std::cerr << file_res.error() << "\n";
        std::terminate();
    }


    auto& file = *file_res;

    if (is_64_bit) {
        if (!edits.empty()) {
            dconstruct::EditDisassembler ed(&file, &base, options, edits);
            ed.apply_file_edits();
        }
    }

    dconstruct::FileDisassembler disassembler(&file, &base, out_disasm_filename.string(), options);
    
    if (is_64_bit) {
        disassembler.disassemble();
    } else {
        disassembler.disassemble_functions_from_bin_file();
    }

    disassembler.dump();
    const auto funcs = disassembler.get_named_functions();
    if (!funcs.empty()) {
        std::ofstream out(out_decomp_filename);
        std::vector<dconstruct::ast::function_definition> functions;
        functions.reserve(funcs.size());
        out << language_type;
        if (use_pascal_case) {
            out << dconstruct::ast::func_pascal_case;
        }
        for (const auto& func : funcs) {
            std::optional<std::filesystem::path> graph_path = std::nullopt;
            if (write_graphs) {
                auto graph_dir = (std::filesystem::path(out_decomp_filename).replace_extension("").concat("_graphs"));
                std::filesystem::create_directories(graph_dir);
                graph_path = get_sanitized_graph_path(graph_dir, func->get_id());
            }
            try {
                functions.emplace_back(dconstruct::dcompiler::decomp_function{ *func, file, dconstruct::ControlFlowGraph::build(*func), std::move(graph_path) }.decompile(optimize));
            }
            catch (const std::exception& e) {
                if (show_warnings) {
                    std::cout << "warning: couldn't decompile <" << func->get_id() << ">: " << e.what() << "\n";
                }
            }
        }
        dconstruct::dcompiler::state_script_functions output_functions{functions, &file};
        output_functions.to_string(out);
    }
}

static void disasm_file(
    const std::filesystem::path &inpath, 
    const std::filesystem::path &out_filename, 
    const dconstruct::SIDBase &base,
    const dconstruct::DisassemblerOptions &options,
    const std::vector<std::string> &edits = {}) {
    
    auto file_res = dconstruct::BinaryFile::from_path(inpath.string());

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

template <bool is_64_bit = true>
static void decompile_multiple(
    const std::filesystem::path &in, 
    const std::filesystem::path &out, 
    const dconstruct::SIDBase &sidbase, 
    const dconstruct::DisassemblerOptions &options,
    const bool generate_graphs,
    const bool show_warnings,
    const bool optimize,
    const dconstruct::ast::print_fn_type language_print,
    const bool pascal_case
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
            decomp_file(entry.string(), disasm_outpath, decomp_outpath, sidbase, options, generate_graphs, language_print, show_warnings, optimize, {}, pascal_case);
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

[[nodiscard]] static std::wstring get_executable_path() {
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

[[nodiscard]] static std::optional<std::pair<cxxopts::Options, cxxopts::ParseResult>> get_command_line_options(int argc, char* argv[]) {
    cxxopts::Options options("dconstruct", "\na program for disassembling, editing and decompiling tlouii dc files. use --about for a more detailed description.\n");

    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    std::filesystem::path current_program_path = converter.to_bytes(dconstruct::disassembly::get_executable_path());

    options.add_options("information")
        ("h, help", "display this message")
        ("help_edit", "help with editing a file")
        ("a,about", "print about");
    options.add_options("input/output")
        ("i,input",  "input DC file or folder", cxxopts::value<std::string>(), "<path>")
        ("o,output", "output file or folder", cxxopts::value<std::string>()->default_value(""), dconstruct::disassembly::DEFAULT_OUT)
        ("s,sidbase", "sidbase file", cxxopts::value<std::string>()->default_value((current_program_path.parent_path() / "sidbase.bin").string()), "<path>");
    options.add_options("configuration")
        ("no_decompile", "don't emit a file containing the decompiled functions (excluding those nested inside structs).", cxxopts::value<bool>()->default_value("false"))
        ("no_optimize", "don't optimize/cleanup the decompiled code output, e.g. replacing some 'for' loops with 'foreach' loops, some if-else chains with match expressions, and removing unused variables.", 
            cxxopts::value<bool>()->default_value("false"))
        ("pascal_case", "convert the games function names into pascal case in the DCPL output.", cxxopts::value<bool>()->default_value("false"))
        ("show_warnings", "don't show warnings for functions that couldn't be decompiled.", cxxopts::value<bool>()->default_value("false"))
        //("language", "specify the DCPL pseudo language type. current options are 'C', 'Racket' (closest to original DC), or 'Python'. default is 'C'.", cxxopts::value<std::string>()->default_value("C"))
        //("shader", "treat the input as a shader file instead.", cxxopts::value<bool>()->default_value("false"))
        ("graphs", "emit control flow graph SVGs of the named functions when decompiling. only emits graphs of size >1. SIGNIFICANTLY slows down decompilation.", cxxopts::value<bool>()->default_value("false"))
        ("emit_once", "only emit the first occurence of a struct. repeating instances will still show the address but not the contents of the struct.", 
            cxxopts::value<bool>()->default_value("false"))
        ("uc4", "experimental: try to disassemble/decompile an uncharted 4 .bin file instead. not tested, so might be very broken.", cxxopts::value<bool>()->default_value("false"));

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
        return std::nullopt;
    }

    return std::pair{options, opts};
}

}