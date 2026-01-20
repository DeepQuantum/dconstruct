#include "cxxopts.hpp"
#include "about.h"
#include "compilation/lexer.h"
#include "compilation/dc_parser.h"
#include "ast/ast.h"
#include <fstream>
#include <sstream>

int run_compilation(const std::string& source_code) {
    const auto start_time = std::chrono::high_resolution_clock::now();
    dconstruct::compiler::Lexer lexer{source_code};
    const auto [tokens, lex_errors] = lexer.get_results();

    if (!lex_errors.empty()) {
        for (const auto& err : lex_errors) {
            std::cerr << "[syntax error] " << err << '\n';
        }
        return -1;
    }

    dconstruct::compiler::Parser parser{tokens};
    const auto [functions, types, parse_errors] = parser.get_results();
    if (!parse_errors.empty()) {
        for (const auto& err : parse_errors) {
            std::cerr << "[parsing error] " << err.m_message << " at line " << err.m_token.m_line << '\n';
        }
        return -1;
    }

    dconstruct::compiler::scope base_scope{};
    base_scope.n_namesToTypes = types;

    for (const auto& func : functions) {
        func.check_semantics(base_scope);
    }
}

int main(int argc, char* argv[]) {
    cxxopts::Options options("dcc, the open source dc compiler", "a compiler for the tlouii dc scripting language.");

    options.add_options("input/output")
        ("i,input",  "input DC file or folder", cxxopts::value<std::string>(), "<path>");

    options.parse_positional({"i"});
    cxxopts::ParseResult opts;
    try {
        opts = options.parse(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return -1;
    }

    std::filesystem::path filepath;
    if (opts.count("i") == 0) {
        std::cerr << "error: no input specified\n";
        return -1;
    } else {
        filepath = opts["i"].as<std::string>();
        
    }

    if (!std::filesystem::exists(filepath)) {
        std::cerr << "error: input filepath " << filepath << " doesn't exist\n";
        return -1;
    }

    std::ifstream file_in{filepath};
    if (!file_in.is_open()) {
        std::cerr << "error: couldn't open " << filepath << "\n";
        return -1;
    }

    std::stringstream input;
    input << file_in.rdbuf();

    return run_compilation(input.str());
}