#include "cxxopts.hpp"
#include "about.h"
#include "compilation/lexer.h"
#include "compilation/dc_parser.h"
#include "binaryfile.h"
#include "disassembly/disassembler.h"
#include "compilation/preprocessor.h"
#include "ast/ast.h"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace dconstruct::compilation {

[[nodiscard]] static std::optional<std::vector<compilation::program_binary_element>> run_compilation(const std::string& source_code, global_state& global) {
    const auto start_time = std::chrono::high_resolution_clock::now();
    Lexer lexer{source_code};
    const auto& [tokens, lex_errors] = lexer.get_results();

    if (!lex_errors.empty()) {
        for (const auto& err : lex_errors) {
            std::cerr << "[syntax error] " << err << '\n';
        }
        return std::nullopt;
    }

    Parser parser{tokens};
    const auto& [program, types, parse_errors] = parser.get_results();
    if (!parse_errors.empty()) {
        for (const auto& err : parse_errors) {
            std::cerr << "[parsing error] " << err.m_message << " at line " << err.m_token.m_line << '\n';
        }
        return std::nullopt;
    }

    scope base_scope{types};
    const std::vector<ast::semantic_check_error> semantic_errors = program.check_semantics(base_scope);
    if (!semantic_errors.empty()) {
        for (const auto& err : semantic_errors) {
            std::cerr << "[semantic error] " << err.m_message << " checking expression " << err.m_expr->to_c_string();  
        }
        return std::nullopt;
    }

    std::expected<std::vector<compilation::program_binary_element>, std::string> compile_res = program.compile_binary_elements(base_scope, global);
    if (!compile_res) {
        std::cerr << "[compilation error] " << compile_res.error() << "\n";
        return std::nullopt;
    }
    return std::move(*compile_res);
}

[[nodiscard]] static std::expected<std::pair<std::unique_ptr<std::byte[]>, u64>, std::string> disassemble_target(
    const std::filesystem::path& target_filepath, 
    const SIDBase& sidbase, 
    const std::vector<compilation::function>& target_functions,
    global_state& global) {

    std::expected<BinaryFile, std::string> file_res = BinaryFile::from_path(target_filepath);
    if (!file_res) {
        return std::unexpected{file_res.error()};
    }

    Disassembler disassembler(&*file_res, &sidbase);
    disassembler.disassemble();

    std::vector<const function_disassembly*> funcs = disassembler.get_named_functions();
    std::ranges::sort(funcs, [](const function_disassembly* a, const function_disassembly* b) {
        return a->m_originalOffset < b->m_originalOffset;
    });

    std::vector<compilation::program_binary_element> converted;

    for (const auto& f : funcs) {
        function cf;
        const std::string id = f->get_id();
        if (id.starts_with("#")) {
            cf.m_name = std::stoull(id.substr(1, id.size() - 1), nullptr, 16);
        } else {
            cf.m_name = id;
        }

        if (cf.m_name == target_functions[0].m_name) {
            cf = target_functions[0];
        } else {
            for (const auto& line : f->m_lines) {
                cf.m_instructions.push_back(line.m_instruction);
            }
            for (u32 i = 0; i < f->m_stackFrame.m_symbolTable.m_types.size(); ++i) {
                const function::SYMBOL_TABLE_POINTER_KIND kind = std::visit([](auto&& type) {
                    using T = std::decay_t<decltype(type)>;
                    if constexpr (std::is_same_v<T, ast::primitive_type>) {
                        return type.m_type == ast::primitive_kind::STRING ? function::SYMBOL_TABLE_POINTER_KIND::STRING : function::SYMBOL_TABLE_POINTER_KIND::NONE;
                    } else if constexpr (std::is_same_v<T, ast::ptr_type>) {
                        return function::SYMBOL_TABLE_POINTER_KIND::GENERAL;
                    } else {
                        return function::SYMBOL_TABLE_POINTER_KIND::NONE;
                    }
                }, f->m_stackFrame.m_symbolTable.m_types[i]);
                if (kind == function::SYMBOL_TABLE_POINTER_KIND::STRING) {
                    const u32 size = global.add_string(f->m_stackFrame.m_symbolTable.m_location.get<const char*>(i * 8));
                    cf.m_symbolTable.push_back(size);
                } else {
                    cf.m_symbolTable.push_back(f->m_stackFrame.m_symbolTable.m_location.get<u64>(i * 8));
                }
                cf.m_symbolTableEntryPointers.push_back(kind);
            }
        }
        converted.push_back(cf.to_binary_element());
    }

    return ast::program::make_binary(std::move(converted), global);
}

[[nodiscard]] static std::optional<cxxopts::ParseResult> get_command_line_options(int argc, char* argv[]) {
    cxxopts::Options options("dcc", "a compiler targeting the dc bytecode used in several NaughtyDog video games.");

    options.add_options("input/output")
        ("i,input",  "input DCPL file that will be compiled", cxxopts::value<std::string>(), "<path>")
        ("t,target",  "the original binary file that will be recompiled", cxxopts::value<std::string>(), "<path>")
        ("o,output",  "output location of the recompiled binary", cxxopts::value<std::string>(), "<path>")
        ("m,modules",  "the modules.bin file that maps this file (either from the original game or from a mod that you're using, like starlight)", cxxopts::value<std::string>(), "<path>")
        ("r,repackage", "path to a the directory containing the .bin files. if provided, the compiler will automatically repackage the directory into a new .psarc file.", cxxopts::value<std::string>(), "<path>")
        ("s,sidbase",  "path to the sidbase", cxxopts::value<std::string>(), "sidbase.bin");

    options.parse_positional({"i"});
    cxxopts::ParseResult opts;
    try {
        opts = options.parse(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return std::nullopt;
    }
    return opts;
}

[[nodiscard]] static std::optional<std::string> patch_modules_size(const std::filesystem::path& modules, const std::filesystem::path& output, const u64 new_size) {
    constexpr std::string_view needle = "/dc1/";
    const auto pos = output.string().find(needle);
    if (pos == std::string_view::npos) {
        return "the path provided needs to have a 'dc1' directory in it, so that the program can figure out the relative path to the modules.";
    }

    std::filesystem::path result = output.string().substr(pos + needle.size());

    const std::string target_name = result.replace_extension("").string();
    const sid64 target_sid = SID(target_name.c_str());

    std::fstream modules_file{modules, std::ios::in | std::ios::out | std::ios::binary};
    if (!modules_file.is_open()) {
        return "couldn't open " + modules.string();
    }

    const u64 modules_size = std::filesystem::file_size(modules);
    std::unique_ptr<std::byte[]> bytes = std::make_unique<std::byte[]>(modules_size);
    modules_file.read(reinterpret_cast<char*>(bytes.get()), modules_size);
    
    bool found = false;
    for (u64 i = 0; i < modules_size && !found; i += 8) {
        const u64 byte_pack = *reinterpret_cast<const u64*>(bytes.get() + i);
        if (byte_pack == target_sid) {
            *reinterpret_cast<u64*>(bytes.get() + i + 8) = new_size;
            found = true;
        }
    }
    if (!found) {
        return "couldn't replace the target name " + target_name + " in the modules.bin file";
    }
    
    modules_file.seekg(0);
    modules_file.write(reinterpret_cast<const char*>(bytes.get()), modules_size);

    return std::nullopt;
}

static i32 create_output(
    const std::expected<dconstruct::compilation::compiler_options, std::string> &filepaths, 
    const dconstruct::SIDBase &sidbase, 
    const std::vector<dconstruct::compilation::function> &functions, 
    dconstruct::compilation::global_state &global
) {
    const auto binary_res = dconstruct::compilation::disassemble_target(filepaths->m_target, sidbase, functions, global);
    if (!binary_res) {
        std::cerr << binary_res.error() << "\n";
        return -1;
    }
    const auto &[bytes, size] = *binary_res;
    std::filesystem::create_directories(filepaths->m_output.parent_path());
    std::ofstream of(filepaths->m_output, std::ios::binary);
    if (!of.is_open()) {
        std::cerr << "couldn't open filepath " << filepaths->m_output << "\n";
        return -1;
    }
    of.write(reinterpret_cast<const char*>(bytes.get()), size);
    of.flush();

    const std::optional<std::string> patch_err = patch_modules_size(filepaths->m_modules, filepaths->m_output, size);
    if (patch_err) {
        std::cerr << *patch_err << "\n";
        return -1;
    }

    return 0;
}

[[nodiscard]] static std::optional<std::string> repackage_psarc(const std::filesystem::path& directory_path) {
    if (!directory_path.string().ends_with("_unpacked/") && !directory_path.string().ends_with("_unpacked")) {
        return "the unpacked directory must end with '_unpacked'";
    }

    std::filesystem::path psarc_path = directory_path.string().substr(0, directory_path.string().size() - sizeof("_unpacked") + 1) + ".psarc"; 


    const std::string command = "ndarc -c \"" + directory_path.string() + "\" -o \"" + psarc_path.string() + "\"";
    std::system(command.c_str());
    return std::nullopt;
}

}