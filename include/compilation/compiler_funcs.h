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
#include <unordered_map>
#include <array>
#include <string_view>
#include <chrono>
#include <algorithm>

namespace dconstruct::compilation {

struct global_function {
    std::string_view m_name;
    ast::function_type m_type;
};

const std::array global_functions = {
    global_function{
        "#display",
        ast::make_function(ast::make_type_from_prim(ast::primitive_kind::NOTHING), ast::function_type::DISTANCE::FAR, {
            {"message", ast::make_type_from_prim(ast::primitive_kind::STRING)},
            {"channel", ast::make_type_from_prim(ast::primitive_kind::U16)}
        })
    },
    global_function{
        "#go",
        ast::make_function(ast::make_type_from_prim(ast::primitive_kind::NOTHING), ast::function_type::DISTANCE::FAR, {
            {"target", ast::make_type_from_prim(ast::primitive_kind::U16)},
            {"mode", ast::make_type_from_prim(ast::primitive_kind::U16)}
        })
    },
    global_function{
        "#dc:format",
        ast::make_function(ast::make_type_from_prim(ast::primitive_kind::STRING), ast::function_type::DISTANCE::FAR, {
            {"format", ast::make_type_from_prim(ast::primitive_kind::STRING)}
        }, true)
    }
};

void add_global_functions(compilation::scope& scope) {
    for (const auto& function : global_functions) {
        const std::string name{function.m_name};
        scope.define(name, function.m_type);
        scope.m_sidAliases[name] = {SID(name.c_str() + 1), name};
    }
}

struct compilation_run_result {
    std::vector<compilation::program_binary_element> m_elements;
    std::chrono::milliseconds m_duration{};
    std::vector<std::string> m_stateScriptNames;
};

[[nodiscard]] static std::optional<compilation_run_result> run_compilation_detailed(
    const std::string& source_code,
    global_state& global,
    const std::vector<source_location>& line_map) {
    const auto start_time = std::chrono::high_resolution_clock::now();
    Lexer lexer{source_code, &line_map};
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
            std::cerr << "[parsing error] " << err.m_message << " at " << format_source_location({err.m_token.m_file, err.m_token.m_line}) << '\n';
        }
        return std::nullopt;
    }

    scope base_scope{types};

    add_global_functions(base_scope);

    const std::vector<ast::semantic_check_error> semantic_errors = program.check_semantics(base_scope);
    if (!semantic_errors.empty()) {
        for (const auto& err : semantic_errors) {
            std::cerr << "[semantic error] " << err.m_message;   
            if (err.m_expr) {
                std::cerr << " checking expression " << err.m_expr->to_c_string();
            }
            std::cerr << "\n";
        }
        return std::nullopt;
    }

    std::vector<std::string> state_script_names;
    for (const ast::global_decl_uptr& declaration : program.m_declarations) {
        if (const auto* state_script = dynamic_cast<const ast::state_script*>(declaration.get())) {
            state_script_names.push_back(state_script->m_name);
        }
    }

    std::expected<std::vector<compilation::program_binary_element>, std::string> compile_res = program.compile_binary_elements(base_scope, global);
    if (!compile_res) {
        std::cerr << "[compilation error] " << compile_res.error() << "\n";
        return std::nullopt;
    }
    const auto end_time = std::chrono::high_resolution_clock::now();
    return compilation_run_result{
        std::move(*compile_res),
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time),
        std::move(state_script_names)
    };
}

[[nodiscard]] static std::optional<std::vector<compilation::program_binary_element>> run_compilation(
    const std::string& source_code,
    global_state& global,
    const std::vector<source_location>& line_map) {
    std::optional<compilation_run_result> detailed = run_compilation_detailed(source_code, global, line_map);
    if (!detailed) {
        return std::nullopt;
    }
    return std::move(detailed->m_elements);
}

[[nodiscard]] static std::optional<std::vector<compilation::program_binary_element>> run_compilation(const std::string& source_code, global_state& global) {
    const std::vector<source_location> line_map;
    return run_compilation(source_code, global, line_map);
}

[[nodiscard]] static std::expected<std::pair<std::unique_ptr<std::byte[]>, u64>, std::string> disassemble_target(
    const std::filesystem::path& target_filepath, 
    const SIDBase& sidbase, 
    const std::vector<compilation::program_binary_element>& target_elements,
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

    std::unordered_map<sid64, const compilation::program_binary_element*> target_elements_by_sid;
    target_elements_by_sid.reserve(target_elements.size());
    for (const compilation::program_binary_element& target_element : target_elements) {
        target_elements_by_sid.emplace(target_element.m_entry.m_nameID, &target_element);
    }

    std::vector<compilation::program_binary_element> converted;

    for (const auto& f : funcs) {
        function cf{};
        const std::string id = f->get_id();
        if (id.starts_with("#")) {
            cf.m_name = std::stoull(id.substr(1, id.size() - 1), nullptr, 16);
        } else {
            cf.m_name = id;
        }

        const sid64 disassembled_function_name_id = std::holds_alternative<sid64>(cf.m_name)
            ? std::get<sid64>(cf.m_name)
            : SID(std::get<std::string>(cf.m_name).c_str());

        const auto replacement_it = target_elements_by_sid.find(disassembled_function_name_id);
        if (replacement_it != target_elements_by_sid.end()) {
            converted.push_back(*replacement_it->second);
            continue;
        }

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
        converted.push_back(cf.to_binary_element());
    }

    return ast::program::make_binary(std::move(converted), global);
}

[[nodiscard]] static std::optional<cxxopts::ParseResult> get_command_line_options(int argc, char* argv[]) {
    cxxopts::Options options("dcc", "a compiler targeting the dc bytecode used in several NaughtyDog video games.");

    options.add_options("input/output")
        ("i,input",  "input DCPL file that will be compiled", cxxopts::value<std::string>(), "<path>")
        ("t,target",  "the original binary file that will be recompiled", cxxopts::value<std::string>(), "<path>")
        ("o,output",  "output location of the recompiled binary; relative to <mod>/bin/dc1 when --mod is provided", cxxopts::value<std::string>(), "<path>")
        ("mod", "path to the mod directory; derives output, modules.bin, pak68.txt, and repackage location", cxxopts::value<std::string>(), "<path>")
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

struct modules_patch_summary {
    std::filesystem::path m_path;
    std::string m_targetName;
    u64 m_oldSize = 0;
    u64 m_newSize = 0;
    u64 m_offset = 0;
};

struct compiler_output_summary {
    std::filesystem::path m_output;
    u64 m_size = 0;
    std::optional<modules_patch_summary> m_modulesPatch;
};

[[nodiscard]] static std::vector<pak68_edit_request> pak68_edits_for_compile(
    const compiler_options& options,
    const std::vector<std::string>& state_script_names) {

    std::vector<pak68_edit_request> edits = options.m_pak68Edits;
    for (const std::string& state_script_name : state_script_names) {
        edits.push_back(pak68_edit_request{
            "sp-all",
            {pak68_entry{pak68_type::SYMBOL, state_script_name}},
            {}
        });
    }
    return edits;
}

[[nodiscard]] static std::string normalized_generic_path_string(const std::filesystem::path& path) {
    std::string normalized = path.generic_string();
    std::ranges::replace(normalized, '\\', '/');
    return normalized;
}

[[nodiscard]] static std::expected<modules_patch_summary, std::string> patch_modules_size(const std::filesystem::path& modules, const std::filesystem::path& output, const u64 new_size) {
    constexpr std::string_view needle = "/bin/dc1/";
    const std::string normalized_output = normalized_generic_path_string(output);
    const auto pos = normalized_output.find(needle);
    if (pos == std::string_view::npos) {
        return std::unexpected{"the path provided needs to have a 'bin/dc1' directory in it, so that the program can figure out the relative path to the modules."};
    }

    std::filesystem::path result = normalized_output.substr(pos + needle.size());

    const std::string target_name = result.replace_extension("").generic_string();
    const sid64 target_sid = SID(target_name.c_str());

    std::fstream modules_file{modules, std::ios::in | std::ios::out | std::ios::binary};
    if (!modules_file.is_open()) {
        return std::unexpected{"couldn't open " + modules.string()};
    }

    const u64 modules_size = std::filesystem::file_size(modules);
    std::unique_ptr<std::byte[]> bytes = std::make_unique<std::byte[]>(modules_size);
    modules_file.read(reinterpret_cast<char*>(bytes.get()), modules_size);
    
    std::optional<modules_patch_summary> summary;
    for (u64 i = 0; i + 16 <= modules_size && !summary; i += 8) {
        const u64 byte_pack = *reinterpret_cast<const u64*>(bytes.get() + i);
        if (byte_pack == target_sid) {
            const u64 old_size = *reinterpret_cast<const u64*>(bytes.get() + i + 8);
            *reinterpret_cast<u64*>(bytes.get() + i + 8) = new_size;
            summary = modules_patch_summary{modules, target_name, old_size, new_size, i + 8};
        }
    }
    if (!summary) {
        return std::unexpected{"couldn't replace the target name " + target_name + " in the modules.bin file"};
    }
    
    modules_file.seekg(0);
    modules_file.write(reinterpret_cast<const char*>(bytes.get()), modules_size);

    return *summary;
}

static std::expected<compiler_output_summary, std::string> create_output(
    const std::expected<dconstruct::compilation::compiler_options, std::string> &filepaths, 
    const dconstruct::SIDBase &sidbase, 
    const std::vector<dconstruct::compilation::program_binary_element> &functions, 
    dconstruct::compilation::global_state &global
) {
    const auto binary_res = filepaths->m_standalone ? 
        dconstruct::ast::program::make_binary(functions, global) 
        : dconstruct::compilation::disassemble_target(filepaths->m_target, sidbase, functions, global);
        
    if (!binary_res) {
        return std::unexpected{binary_res.error()};
    }
    const auto &[bytes, size] = *binary_res;
    std::filesystem::create_directories(filepaths->m_output.parent_path());
    std::ofstream of(filepaths->m_output, std::ios::binary);
    if (!of.is_open()) {
        return std::unexpected{"couldn't open filepath " + filepaths->m_output.string()};
    }
    of.write(reinterpret_cast<const char*>(bytes.get()), size);
    of.flush();

    compiler_output_summary summary;
    summary.m_output = filepaths->m_output;
    summary.m_size = size;

    if (!filepaths->m_modules.empty()) {
        std::expected<modules_patch_summary, std::string> patch = patch_modules_size(filepaths->m_modules, filepaths->m_output, size);
        if (!patch) {
            return std::unexpected{patch.error()};
        }
        summary.m_modulesPatch = std::move(*patch);
    }

    return summary;
}

[[nodiscard]] static std::expected<std::filesystem::path, std::string> repackage_psarc(const std::filesystem::path& directory_path) {
    if (!directory_path.string().ends_with("_unpacked/") && !directory_path.string().ends_with("_unpacked")) {
        return std::unexpected{"the unpacked directory must end with '_unpacked'"};
    }

    std::filesystem::path psarc_path = directory_path.string().substr(0, directory_path.string().size() - sizeof("_unpacked") + 1) + ".psarc"; 


    const std::string command = "ndarc -c \"" + directory_path.string() + "\" -o \"" + psarc_path.string() + "\"";
    const int result = std::system(command.c_str());
    if (result != 0) {
        return std::unexpected{"ndarc failed while producing " + psarc_path.string()};
    }
    return psarc_path;
}

}
