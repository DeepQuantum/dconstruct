#include "cxxopts.hpp"
#include "about.h"
#include "compilation/lexer.h"
#include "compilation/dc_parser.h"
#include "binaryfile.h"
#include "disassembly/disassembler.h"
#include "ast/ast.h"
#include <fstream>
#include <sstream>
#include <filesystem>

std::optional<std::tuple<std::vector<dconstruct::compilation::function>, std::unique_ptr<std::byte[]>, u64>> run_compilation(const std::string& source_code) {
    const auto start_time = std::chrono::high_resolution_clock::now();
    dconstruct::compilation::Lexer lexer{source_code};
    const auto& [tokens, lex_errors] = lexer.get_results();

    if (!lex_errors.empty()) {
        for (const auto& err : lex_errors) {
            std::cerr << "[syntax error] " << err << '\n';
        }
        return std::nullopt;
    }

    dconstruct::compilation::Parser parser{tokens};
    const auto& [program, types, parse_errors] = parser.get_results();
    if (!parse_errors.empty()) {
        for (const auto& err : parse_errors) {
            std::cerr << "[parsing error] " << err.m_message << " at line " << err.m_token.m_line << '\n';
        }
        return std::nullopt;
    }

    dconstruct::compilation::scope base_scope{types};
    const std::vector<dconstruct::ast::semantic_check_error> semantic_errors = program.check_semantics(base_scope);
    if (!semantic_errors.empty()) {
        for (const auto& err : semantic_errors) {
            std::cerr << "[semantic error] " << err.m_message << " checking expression " << err.m_expr->to_c_string();  
        }
    }

    std::expected<dconstruct::ast::program::compile_res, std::string> compile_res = program.compile(base_scope);
    if (!compile_res) {
        std::cerr << "[compilation error] " << compile_res.error() << "\n";
    }
    return std::move(*compile_res);
}

void disassemble_target(const std::filesystem::path& target_filepath, const dconstruct::SIDBase& sidbase, const std::vector<dconstruct::compilation::function>& target_functions) {
    std::expected<dconstruct::BinaryFile<>, std::string> file_res = dconstruct::BinaryFile<>::from_path(target_filepath);
    if (!file_res) {
        return;
    }

    dconstruct::Disassembler<true> disassembler(&*file_res, &sidbase);
    disassembler.disassemble();

    std::vector<dconstruct::function_disassembly> funcs = disassembler.get_functions();
    std::ranges::sort(funcs, [](const dconstruct::function_disassembly& a, const dconstruct::function_disassembly& b) {
        return a.m_originalOffset < b.m_originalOffset;
    });

    std::vector<dconstruct::compilation::function> converted;
    dconstruct::compilation::global_state global;

    for (const auto& f : funcs) {
        dconstruct::compilation::function cf;
        const std::string id = f.get_id();
        if (id.starts_with("#")) {
            cf.m_name = std::stoull(id.substr(1, id.size() - 1), nullptr, 16);
        } else {
            cf.m_name = id;
        }
        
        if (cf.m_name == target_functions[0].m_name) {
            cf.m_instructions = target_functions[0].m_instructions;
            cf.m_symbolTable = target_functions[0].m_symbolTable;
        } else {
            for (const auto& line : f.m_lines) {
                cf.m_instructions.push_back(line.m_instruction);
            }
            for (u32 i = 0; i < f.m_stackFrame.m_symbolTable.m_types.size(); ++i) {
                const dconstruct::compilation::function::SYMBOL_TABLE_POINTER_KIND kind = std::visit([](auto&& type) {
                    using T = std::decay_t<decltype(type)>;
                    if constexpr (std::is_same_v<T, dconstruct::ast::primitive_type>) {
                        return type.m_type == dconstruct::ast::primitive_kind::STRING ? dconstruct::compilation::function::SYMBOL_TABLE_POINTER_KIND::STRING : dconstruct::compilation::function::SYMBOL_TABLE_POINTER_KIND::NONE;
                    } else if constexpr (std::is_same_v<T, dconstruct::ast::ptr_type>) {
                        return dconstruct::compilation::function::SYMBOL_TABLE_POINTER_KIND::GENERAL;
                    } else {
                        return dconstruct::compilation::function::SYMBOL_TABLE_POINTER_KIND::NONE;
                    }
                }, f.m_stackFrame.m_symbolTable.m_types[i]);
                if (kind == dconstruct::compilation::function::SYMBOL_TABLE_POINTER_KIND::STRING) {
                    const u32 size = global.add_string(f.m_stackFrame.m_symbolTable.m_location.get<const char*>(i * 8));
                    cf.m_symbolTable.push_back(size);
                } else {
                    cf.m_symbolTable.push_back(f.m_stackFrame.m_symbolTable.m_location.get<u64>(i * 8));
                }
                cf.m_symbolTableEntryPointers.push_back(kind);
            }
        }
        converted.push_back(std::move(cf));
    }
}

int main(int argc, char* argv[]) {
    cxxopts::Options options("dcc", "a compiler targeting the dc bytecode used in several NaughtyDog video games.");

    options.add_options("input/output")
        ("i,input",  "input DCPL file that will be compiled", cxxopts::value<std::string>(), "<path>")
        ("t,target",  "the original binary file that will be recompiled", cxxopts::value<std::string>(), "<path>")
        ("o,output",  "output location of the recompiled binary", cxxopts::value<std::string>(), "<path>")
        ("s,sidbase",  "path to the sidbase", cxxopts::value<std::string>(), "sidbase.bin");

    options.parse_positional({"i"});
    cxxopts::ParseResult opts;
    try {
        opts = options.parse(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return -1;
    }

    std::filesystem::path source_code_filepath;
    if (opts.count("i") == 0) {
        std::cerr << "error: no input specified\n";
        return -1;
    } else {
        source_code_filepath = opts["i"].as<std::string>();
    }

    if (!std::filesystem::exists(source_code_filepath)) {
        std::cerr << "error: input filepath " << source_code_filepath << " doesn't exist\n";
        return -1;
    }

    std::filesystem::path target_binary_filepath;
    if (opts.count("t") == 0) {
        std::cerr << "error: no target binary specified\n";
        return -1;
    } else {
        target_binary_filepath = opts["t"].as<std::string>();
    }

    if (!std::filesystem::exists(target_binary_filepath)) {
        std::cerr << "error: input filepath " << target_binary_filepath << " doesn't exist\n";
        return -1;
    }

    std::filesystem::path output_filepath;
    if (opts.count("o") == 0) {
        std::cerr << "error: no output specified specified\n";
        return -1;
    } else {
        output_filepath = opts["o"].as<std::string>();
    }

    std::ifstream file_in{source_code_filepath};
    if (!file_in.is_open()) {
        std::cerr << "error: couldn't open " << source_code_filepath << "\n";
        return -1;
    }

    std::stringstream input;
    input << file_in.rdbuf();

    if (opts.count("s") == 0) {

    } 

    std::filesystem::path sidbase_path = opts["s"].as<std::string>();
    const std::expected<dconstruct::SIDBase, std::string> sidbase_res = dconstruct::SIDBase::from_binary(sidbase_path);
    if (!sidbase_res) {
        std::cerr << sidbase_res.error() << "\n";
        return -1;
    }
    const dconstruct::SIDBase& sidbase = *sidbase_res;

    const auto result = run_compilation(input.str());
    if (!result) {
        return -1;
    }

    const auto& [functions, bytes, size] = *result;
    disassemble_target(target_binary_filepath, sidbase, functions);

}