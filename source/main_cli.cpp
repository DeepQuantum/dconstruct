#include "disassembly/driver_functions.h"

#include <cstdio>
#include <print>

int main(int argc, char *argv[]) {  
    const std::optional<std::pair<cxxopts::Options, cxxopts::ParseResult>> opts_res = dconstruct::disassembly::get_command_line_options(argc, argv);
    if (!opts_res) {
        return -1;
    }
    const auto& [options, opts] = *opts_res;

    if (opts.count("h") > 0) {
        std::println("{}", options.help({"", "information", "input/output", "configuration", "edit"}));
        return 0;
    }

    if (opts.count("help_edit") > 0) {
        std::print("{}", HELP_EDIT);
        return 0;
    }

    if (opts.count("a") > 0) {
        print_about();
        return 0;
    }
    
    std::filesystem::path filepath;
    if (opts.count("i") == 0) {
        std::println(stderr, "error: no input specified");
        return -1;
    } else {
        filepath = opts["i"].as<std::string>();
        if (!std::filesystem::exists(filepath)) {
            std::println(stderr, "error: input filepath {} doesn't exist", filepath.string());
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
        if (std::filesystem::is_directory(output) && !std::filesystem::is_directory(filepath)) {
            output /= filepath.filename().string() + ".asm";
        }
    }

    const bool output_is_folder = std::filesystem::is_directory(output);

    const std::filesystem::path sidbase_path = opts["s"].as<std::string>();
    const std::string game_type_string = opts["game_type"].as<std::string>();
    const auto game_type_opt = dconstruct::disassembly::get_game_type(game_type_string);

    if (!game_type_opt) {
        std::println(stderr, "error: unknown game type: '{}'", game_type_string);
        return -1;
    }

    const dconstruct::game_type game = *game_type_opt;

    if (!std::filesystem::exists(sidbase_path)) {
        std::println(stderr, "error: sidbase path {} doesn't exist", sidbase_path.string());
        return -1;
    }
    
    const bool map_types = opts["map_types"].as<bool>();

    if (map_types) {
        if (std::filesystem::is_directory(output)) {
            std::println(stderr, "error: output path must be a file when using --map_types");
            return -1;
        } 
        if (!std::filesystem::is_directory(filepath)) {
            std::println(stderr, "error: input path must be a folder when using --map_types");
            return -1;
        }
        const std::expected<dconstruct::SIDBase, std::string> sidbase_opt = dconstruct::SIDBase::from_binary(sidbase_path);
        if (!sidbase_opt) {
            std::println(stderr, "error: couldn't load sidbase: {}", sidbase_opt.error());
            return -1;
        }
        dconstruct::disassembly::map_types_multiple_to_file(filepath, *sidbase_opt, output, game);
        return 0;
    }

    const bool decompile = !opts["no_decompile"].as<bool>();
    const bool optimize = !opts["no_optimize"].as<bool>();
    const bool generate_graphs = opts["graphs"].as<bool>();
    const bool show_warnings = opts["show_warnings"].as<bool>();



    const std::string language_type = opts["language"].as<std::string>();

    const auto opt_print_func = dconstruct::disassembly::get_print_type(language_type);
    if (!opt_print_func) {
        std::println(stderr, "error: unknown language type: '{}'", language_type);
        return -1;
    }
    auto language_flags = *opt_print_func;
    if (opts["pascal_case"].as<bool>()) {
        language_flags = language_flags | dconstruct::ast::LANGUAGE_FLAGS::FUNCTION_NAMES_PASCAL;
    }


    if (opts.count("e") > 0) {
        std::vector<std::string> edit_strings = opts["e"].as<std::vector<std::string>>();
        edits.insert(edits.end(), edit_strings.begin(), edit_strings.end());
    }

    auto base_exp = dconstruct::SIDBase::from_binary(sidbase_path);
    if (!base_exp) {
        std::print(stderr, "{}", base_exp.error());
        return -1;
    }
    const auto& base = *base_exp;

    if (std::filesystem::is_directory(filepath)) {
        if (std::filesystem::exists(output) && !output_is_folder) {
            std::println(stderr, "error: the input {} is a folder, but output {} is a file.", filepath.string(), output.string());
            return -1;
        }
        if (!output_is_folder) {
            std::filesystem::create_directories(output);
        }
        if (!edits.empty()) {
            std::println("warning: edits ignored as input path is a directory. edits only work in single file disassembly.");
        }
        if (decompile) {
            if (generate_graphs) {
                std::filesystem::create_directory(output / "graphs");
            }
            dconstruct::disassembly::decompile_multiple(filepath, output, base, generate_graphs, show_warnings, optimize, language_flags, game);
        }
        else {
            dconstruct::disassembly::disassemble_multiple(filepath, output, base, game);
        }
    } else {
        const auto start = std::chrono::high_resolution_clock::now();
        if (decompile) {
            std::println("disassembling & decompiling {}...", filepath.filename().string());
            dconstruct::disassembly::decomp_file(filepath, output, std::filesystem::path(output).replace_extension(".dcpl"), base, generate_graphs, language_flags, show_warnings, optimize ? dconstruct::dcompiler::OPTIMIZATION_KIND::AST : dconstruct::dcompiler::OPTIMIZATION_KIND::NONE, edits, game);
        }
        else {
            std::println("disassembling {}...", filepath.filename().string());
            dconstruct::disassembly::disasm_file(filepath, output, base, edits, game);
        }
        const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
        std::println("took {}ms", time_taken.count());
    }
    return 0;
}
