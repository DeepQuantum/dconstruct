#include "compilation/compiler_funcs.h"


int main(int argc, char* argv[]) {
    
    const std::optional<cxxopts::ParseResult> options = dconstruct::compilation::get_command_line_options(argc, argv);
    if (!options) {
        return -1;
    }
    const cxxopts::ParseResult opts = *options;

    std::filesystem::path source_code_filepath;
    if (opts.count("i") == 0) {
        std::cerr << "error: no input specified\n";
        return -1;
    } else {
        source_code_filepath = opts["i"].as<std::string>();
    }
    std::ifstream file_in{source_code_filepath};
    if (!file_in.is_open()) {
        std::cerr << "error: couldn't open " << source_code_filepath << "\n";
        return -1;
    }

    std::stringstream input;
    input << file_in.rdbuf();
    
    std::string source = input.str();

    const std::expected<dconstruct::compilation::compiler_options, std::string> compiler_options_res = dconstruct::compilation::compiler_options::parse(opts, source);
    if (!compiler_options_res) {
        std::cerr << compiler_options_res.error() << "\n";
        return -1;
    }

    const std::expected<dconstruct::SIDBase, std::string> sidbase_res = dconstruct::SIDBase::from_binary(compiler_options_res->m_sidbase);
    if (!sidbase_res) {
        std::cerr << sidbase_res.error() << "\n";
        return -1;
    }

    const dconstruct::SIDBase& sidbase = *sidbase_res;

    dconstruct::compilation::global_state global;

    const auto function_res = dconstruct::compilation::run_compilation(source, global);
    if (!function_res) {
        return -1;
    }

    const auto& functions = *function_res;
    if (create_output(compiler_options_res, sidbase, functions, global) != 0) {
        return -1;
    }


    if (compiler_options_res->m_repackage) {
        const std::optional<std::string> repackage_err = dconstruct::compilation::repackage_psarc(*compiler_options_res->m_repackage);
        if (repackage_err) {
            std::cerr << *repackage_err << "\n";
            return -1;
        }
    }
    return 0;
}


