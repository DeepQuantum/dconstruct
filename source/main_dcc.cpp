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

    const std::expected<dconstruct::compilation::required_options, std::string> filepaths = dconstruct::compilation::required_options::parse(opts, source);
    if (!filepaths) {
        std::cerr << filepaths.error() << "\n";
        return -1;
    }

    const std::expected<dconstruct::SIDBase, std::string> sidbase_res = dconstruct::SIDBase::from_binary(filepaths->m_sidbase);
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
    if (create_output(filepaths, sidbase, functions, global) != 0) {
        return -1;
    }
}


