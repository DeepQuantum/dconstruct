#include "disassembly/disassembly_functions.h"

int main(int argc, char *argv[]) {

    const std::optional<std::pair<cxxopts::Options, cxxopts::ParseResult>> opts_res = dconstruct::disassembly::get_command_line_options(argc, argv);
    if (!opts_res) {
        return -1;
    }
    const auto& [options, opts] = *opts_res;

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

    // if (opts.count("shader") > 0) {
    //    return disassemble_shader(filepath);
    // }

    std::vector<std::string> edits{};
    if (opts.count("edit_file") > 0) {
        std::string test = opts["edit_file"].as<std::string>();
        edits = dconstruct::disassembly::edits_from_file(test);
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

    constexpr u8 indent_per_level = 2;// opts["indent"].as<u8>();
    const bool emit_once = opts["emit_once"].as<bool>();
    const bool decompile = !opts["no_decompile"].as<bool>();
    const bool optimize = !opts["no_optimize"].as<bool>();
    const bool generate_graphs = opts["graphs"].as<bool>();
    const bool use_pascal_case = opts["pascal_case"].as<bool>();
    const bool show_warnings = opts["show_warnings"].as<bool>();
    const bool uc4 = opts["uc4"].as<bool>();
    // const std::string language_type = "C"; // opts["language"].as<std::string>();

    // const auto opt_print_func = get_print_type(language_type);
    // if (!opt_print_func) {
    //     std::cerr << "error: unknown language type: '" << language_type << "'\n";
    //     return -1;
    // }
    const auto& print_func = dconstruct::ast::c;;
    

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
            if (uc4) {
                dconstruct::disassembly::decompile_multiple<false>(filepath, output, base, disassember_options, generate_graphs, show_warnings, optimize, print_func, use_pascal_case);
            } else {
                dconstruct::disassembly::decompile_multiple<true>(filepath, output, base, disassember_options, generate_graphs, show_warnings, optimize, print_func, use_pascal_case);
            }
        }
        else {
            dconstruct::disassembly::disassemble_multiple(filepath, output, base, disassember_options);
        }
    } else {
        const auto start = std::chrono::high_resolution_clock::now();
        if (decompile) {
            std::cout << "disassembling & decompiling " << filepath.filename() << "...\n";
            dconstruct::disassembly::decomp_file(filepath, output, std::filesystem::path(output).replace_extension(".dcpl"), base, disassember_options, generate_graphs, print_func, show_warnings, optimize, edits, use_pascal_case, !uc4);
        }
        else {
            std::cout << "disassembling " << filepath.filename() << "...\n";
            dconstruct::disassembly::disasm_file(filepath, output, base, disassember_options, edits);
        }
        const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
        std::cout << "took " << time_taken.count() << "ms\n";
    }
    return 0;
}