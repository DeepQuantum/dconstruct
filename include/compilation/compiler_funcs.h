#pragma once

#include "ast/type.h"
#include "compilation/function.h"
#include "cxxopts.hpp"
#include "about.h"
#include "compilation/lexer.h"
#include "compilation/dc_parser.h"
#include "binaryfile.h"
#include "disassembly/disassembler.h"
#include "compilation/preprocessor.h"
#include "compilation/target_patcher.h"
#include "ast/ast.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <string_view>
#include <chrono>
#include <algorithm>
#include <cstring>
#include <format>

namespace dconstruct::compilation {

    struct global_function {
        std::string_view m_name;
        ast::function_type m_type;
    };

    const std::array global_functions = {
        global_function{
            "#display",
            ast::make_function(
                ast::make_type_from_prim(ast::primitive_kind::NOTHING),
                ast::function_type::DISTANCE::FAR,
                {
                    {"message", ast::make_type_from_prim(ast::primitive_kind::STRING)},
                }
            )},
        global_function{"#go", ast::make_function(ast::make_type_from_prim(ast::primitive_kind::NOTHING), ast::function_type::DISTANCE::FAR, {{"target", ast::make_type_from_prim(ast::primitive_kind::U16)}, {"mode", ast::make_type_from_prim(ast::primitive_kind::U16)}})}, global_function{"#dc:format", ast::make_function(ast::make_type_from_prim(ast::primitive_kind::STRING), ast::function_type::DISTANCE::FAR, {{"format", ast::make_type_from_prim(ast::primitive_kind::STRING)}}, true)}};

    inline void add_global_functions(compilation::scope& scope) {
        for (const auto& function : global_functions) {
            const std::string name{function.m_name};
            scope.define(name, function.m_type);
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
        const std::vector<source_location>& line_map
    ) {
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
        auto [program, types, parse_errors] = parser.get_results();
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
                    if (const std::optional<source_location> loc = err.m_expr->source_location()) {
                        std::cerr << " at " << format_source_location(*loc);
                    }
                    std::cerr << " checking expression " << err.m_expr->to_pseudo_c_string();
                }
                std::cerr << "\n";
            }
            return std::nullopt;
        }

        std::vector<std::string> state_script_names;
        for (u64 i = 0; i < program.m_declarations.size(); ++i) {
            if (const auto* state_script = dynamic_cast<const ast::state_script*>(program.m_declarations[i].get())) {
                state_script_names.push_back(state_script->m_name);
                if (i == 0)
                    break;
                program.m_declarations[0].swap(program.m_declarations[i]);
                break;
            }
        }

        resstr<std::vector<compilation::program_binary_element>> compile_res = program.compile_binary_elements(base_scope, global);
        if (!compile_res) {
            std::cerr << "[compilation error] " << compile_res.error() << "\n";
            return std::nullopt;
        }
        const auto end_time = std::chrono::high_resolution_clock::now();
        return compilation_run_result{
            std::move(*compile_res),
            std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time),
            std::move(state_script_names)};
    }

    [[nodiscard]] static std::optional<std::vector<compilation::program_binary_element>> run_compilation(
        const std::string& source_code,
        global_state& global,
        const std::vector<source_location>& line_map
    ) {
        std::optional<compilation_run_result> detailed = run_compilation_detailed(source_code, global, line_map);
        if (!detailed) {
            return std::nullopt;
        }
        return std::move(detailed->m_elements);
    }

    [[nodiscard]] static std::optional<std::vector<compilation::program_binary_element>> run_compilation(
        const std::string& source_code,
        global_state& global
    ) {
        const std::vector<source_location> line_map;
        return run_compilation(source_code, global, line_map);
    }

    [[nodiscard]] static resstr<std::pair<std::unique_ptr<std::byte[]>, u64>> disassemble_target(
        const std::filesystem::path& target_filepath,
        const SIDBase& sidbase,
        const std::vector<compilation::program_binary_element>& target_elements,
        global_state& global
    ) {
        resstr<BinaryFile> file_res = BinaryFile::from_path(target_filepath);
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

        std::unordered_set<sid64> target_entry_sids;
        target_entry_sids.reserve(file_res->m_dcheader->m_numEntries);
        const Entry* target_entries = file_res->m_dcheader->m_pStartOfData;
        for (u32 i = 0; i < file_res->m_dcheader->m_numEntries; ++i) {
            target_entry_sids.insert(target_entries[i].m_nameID);
        }

        std::vector<compilation::program_binary_element> patched_elements;
        std::unordered_set<sid64> consumed;
        for (const compilation::program_binary_element& target_element : target_elements) {
            if (target_entry_sids.contains(target_element.m_entry.m_nameID)) {
                patched_elements.push_back(target_element);
                consumed.insert(target_element.m_entry.m_nameID);
            }
        }

        for (const auto& f : funcs) {
            function_context cf{};
            const std::string id = f->get_id();
            if (id.starts_with("#")) {
                cf.m_name = std::stoull(id.substr(1, id.size() - 1), nullptr, 16);
            } else {
                cf.m_name = id;
            }

            const sid64 disassembled_function_name_id = std::holds_alternative<sid64>(cf.m_name)
                                                            ? std::get<sid64>(cf.m_name)
                                                            : SID(std::get<std::string>(cf.m_name).c_str());

            if (!f->m_isScriptFunction) {
                continue;
            }

            const auto replacement_it = target_elements_by_sid.find(disassembled_function_name_id);
            if (replacement_it != target_elements_by_sid.end() && !consumed.contains(disassembled_function_name_id)) {
                if (!f->m_stateScriptLambdaPointerOffset) {
                    return std::unexpected{"matched state script lambda " + id + " but couldn't find its SsLambda pointer slot"};
                }
                compilation::program_binary_element replacement = *replacement_it->second;
                replacement.m_targetPointerOffsets.push_back(*f->m_stateScriptLambdaPointerOffset);
                replacement.m_targetPointerValueOffset = sizeof(sid64);
                patched_elements.push_back(std::move(replacement));
                consumed.insert(disassembled_function_name_id);
            }
        }

        for (const compilation::program_binary_element& target_element : target_elements) {
            if (!consumed.contains(target_element.m_entry.m_nameID)) {
                std::cerr << std::format("[warning] compiled entry #{:016X} matches no entry or state script lambda in the target and will be ignored\n", target_element.m_entry.m_nameID);
            }
        }

        if (patched_elements.empty()) {
            return std::unexpected{"none of the compiled entries match an entry or state script lambda in " + target_filepath.string()};
        }

        return compilation::patch_target_binary(*file_res, std::move(patched_elements), global);
    }

    [[nodiscard]] static std::optional<cxxopts::ParseResult> get_command_line_options(int argc, char* argv[]) {
        cxxopts::Options options("dcc", "a compiler targeting the dc bytecode used in several NaughtyDog video games.");

        options.add_options("input/output")("i,input", "input DCPL file(s) that will be compiled; @game, @mod, and @target are set via directives inside each file", cxxopts::value<std::vector<std::string>>(), "<path>");

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

    struct modules_patch_request {
        std::filesystem::path m_output;
        u64 m_size = 0;
        std::vector<sid64> m_exports;
    };

    struct ModuleInfoArray {
        u32 m_numEntries = 0;
        u32 m_unk = 0;
        ModulesEntry* m_entries = nullptr;
    };

    [[nodiscard]] static std::string filename_without_parent(const std::string& module_path) {
        const auto slash_pos = module_path.find_last_of('/');
        return slash_pos == std::string::npos ? module_path : module_path.substr(slash_pos + 1);
    }

    [[nodiscard]] static std::vector<sid64> default_module_imports() {
        return {
            SID("nd-state-script-syntax"),
            SID("script-funcs-upgrades"),
            SID("script-user-externs"),
            SID("npc-script-funcs"),
            SID("ss-rogue/rogue-misc-defines"),
            SID("script-funcs"),
            SID("nd-script-funcs"),
            SID("game-types"),
            SID("dc-types"),
            SID("state-script-defines"),
        };
    }

    [[nodiscard]] static std::vector<sid64> copy_modules_symbol_array(const SymbolArray* symbol_array) {
        std::vector<sid64> symbols;
        if (symbol_array == nullptr) {
            return symbols;
        }
        symbols.reserve(symbol_array->m_numEntries);
        for (u32 i = 0; i < symbol_array->m_numEntries; ++i) {
            symbols.push_back(symbol_array->m_pSymbols[i]);
        }
        return symbols;
    }

    [[nodiscard]] static std::vector<sid64> default_module_exports(const std::string& target_name) {
        return {SID(filename_without_parent(target_name).c_str())};
    }

    [[nodiscard]] static std::vector<sid64> normalized_module_exports(
        const std::string& target_name,
        const std::vector<sid64>& exports
    ) {
        return exports.empty() ? default_module_exports(target_name) : exports;
    }

    [[nodiscard]] static std::vector<sid64> module_exports_for_elements(const std::vector<program_binary_element>& elements) {
        std::vector<sid64> exports;
        exports.reserve(elements.size());
        for (const program_binary_element& element : elements) {
            exports.push_back(element.m_entry.m_nameID);
        }
        return exports;
    }

    static void patch_element_u64(program_binary_element& element, const u64 offset, const u64 value) {
        std::memcpy(element.m_rawData.data() + offset, &value, sizeof(value));
    }

    struct modules_symbol_array_payload {
        u64 m_symbolArrayOffset = 0;
        const std::vector<sid64>* m_symbols = nullptr;
    };

    [[nodiscard]] static u64 emit_modules_symbol_array_struct(
        program_binary_element& element,
        const std::vector<sid64>& symbols
    ) {
        constexpr sid64 symbol_array_sid = SID("symbol-array");

        element.push_bytes(symbol_array_sid, 0b0);
        const u64 symbol_array_offset = element.m_rawData.size();
        const SymbolArray symbol_array{
            static_cast<u32>(symbols.size()),
            0,
            nullptr,
        };
        element.push_bytes(symbol_array, 0b010);
        return symbol_array_offset;
    }

    static void emit_modules_symbol_array_payload(
        program_binary_element& element,
        const modules_symbol_array_payload& payload
    ) {
        constexpr sid64 array_sid = SID("array");

        element.push_bytes(array_sid, 0b0);
        const u64 symbols_offset = element.m_rawData.size();
        patch_element_u64(element, payload.m_symbolArrayOffset + offsetof(SymbolArray, m_pSymbols), symbols_offset);
        for (const sid64 symbol : *payload.m_symbols) {
            element.push_bytes(symbol, 0b0);
        }
    }

    struct module_entry_source {
        std::string m_name;
        sid64 m_nameSid = 0;
        u64 m_size = 0;
        std::vector<sid64> m_imports;
        std::vector<sid64> m_exports;
        u64 m_rawEntryOffset = 0;
    };

    [[nodiscard]] static std::vector<pak68_edit_request> pak68_edits_for_compile(
        const compiler_options& options,
        const std::vector<std::string>& state_script_names
    ) {
        std::vector<pak68_edit_request> edits = options.m_pak68Edits;
        for (const std::string& state_script_name : state_script_names) {
            edits.push_back(pak68_edit_request{
                "level-name",
                "sp-all",
                {pak68_entry{pak68_type::SYMBOL, state_script_name}},
                {}});
        }
        return edits;
    }

    [[nodiscard]] static std::string normalized_generic_path_string(const std::filesystem::path& path) {
        std::string normalized = path.generic_string();
        std::ranges::replace(normalized, '\\', '/');
        return normalized;
    }

    [[nodiscard]] static resstr<std::string> module_target_name_from_output(const std::filesystem::path& output) {
        constexpr std::string_view needle = "/bin/dc1/";
        const std::string normalized_output = normalized_generic_path_string(output);
        const auto pos = normalized_output.find(needle);
        if (pos == std::string_view::npos) {
            return std::unexpected{"the path provided needs to have a 'bin/dc1' directory in it, so that the program can figure out the relative path to the modules."};
        }

        std::filesystem::path result = normalized_output.substr(pos + needle.size());
        return result.replace_extension("").generic_string();
    }

    struct modules_patch_target {
        std::string m_name;
        sid64 m_sid = 0;
        u64 m_newSize = 0;
        u64 m_oldSize = 0;
        std::vector<sid64> m_exports;
    };

    [[nodiscard]] static resstr<std::unordered_set<sid64>> read_modules_entry_sids(const std::filesystem::path& modules) {
        resstr<BinaryFile> file_res = BinaryFile::from_path(modules);
        if (!file_res) {
            return std::unexpected{file_res.error()};
        }
        const BinaryFile& file = *file_res;

        constexpr sid64 module_info_array_sid = SID("module-info-array");
        if (file.m_dcheader->m_numEntries != 1) {
            return std::unexpected{"modules.bin is expected to contain exactly one entry"};
        }

        const Entry* file_entry = file.m_dcheader->m_pStartOfData;
        if (file_entry == nullptr || file_entry->m_typeId != module_info_array_sid || file_entry->m_entryPtr == nullptr) {
            return std::unexpected{"modules.bin does not contain a module-info-array entry"};
        }

        const auto* module_array = reinterpret_cast<const ModuleInfoArray*>(file_entry->m_entryPtr);
        if (module_array->m_entries == nullptr) {
            return std::unexpected{"modules.bin module-info-array does not point to a ModulesEntry array"};
        }

        std::unordered_set<sid64> sids;
        sids.reserve(module_array->m_numEntries);
        for (u32 i = 0; i < module_array->m_numEntries; ++i) {
            sids.insert(module_array->m_entries[i].m_nameSid);
        }
        return sids;
    }

    [[nodiscard]] static resstr<std::vector<modules_patch_summary>> patch_modules_sizes(
        const std::filesystem::path& modules,
        const std::vector<modules_patch_request>& requests
    ) {
        if (requests.empty()) {
            return std::vector<modules_patch_summary>{};
        }

        std::vector<modules_patch_target> targets;
        targets.reserve(requests.size());
        std::unordered_map<sid64, u64> target_indices;
        for (const modules_patch_request& request : requests) {
            resstr<std::string> target_name = module_target_name_from_output(request.m_output);
            if (!target_name) {
                return std::unexpected{target_name.error()};
            }
            const sid64 target_sid = SID(target_name->c_str());
            if (target_indices.contains(target_sid)) {
                return std::unexpected{"modules.bin target " + *target_name + " was provided multiple times"};
            }
            target_indices[target_sid] = targets.size();
            targets.push_back(modules_patch_target{
                std::move(*target_name),
                target_sid,
                request.m_size,
                0,
                request.m_exports,
            });
        }

        resstr<BinaryFile> file_res = BinaryFile::from_path(modules);
        if (!file_res) {
            return std::unexpected{file_res.error()};
        }
        BinaryFile& file = *file_res;

        constexpr sid64 module_info_array_sid = SID("module-info-array");
        if (file.m_dcheader->m_numEntries != 1) {
            return std::unexpected{"modules.bin is expected to contain exactly one entry"};
        }

        Entry* file_entry = file.m_dcheader->m_pStartOfData;
        if (file_entry == nullptr || file_entry->m_typeId != module_info_array_sid || file_entry->m_entryPtr == nullptr) {
            return std::unexpected{"modules.bin does not contain a module-info-array entry"};
        }

        auto* module_array = const_cast<ModuleInfoArray*>(reinterpret_cast<const ModuleInfoArray*>(file_entry->m_entryPtr));
        if (module_array->m_entries == nullptr) {
            return std::unexpected{"modules.bin module-info-array does not point to a ModulesEntry array"};
        }

        std::vector<module_entry_source> entries;
        entries.reserve(static_cast<u64>(module_array->m_numEntries) + requests.size());
        std::unordered_map<sid64, u64> entry_indices;
        for (u32 i = 0; i < module_array->m_numEntries; ++i) {
            const ModulesEntry& old_entry = module_array->m_entries[i];
            if (old_entry.m_name == nullptr) {
                return std::unexpected{"modules.bin contains a module entry with a null name"};
            }
            std::vector<sid64> exports = copy_modules_symbol_array(old_entry.m_exports);
            u64 size = old_entry.m_size;
            if (const auto target_it = target_indices.find(old_entry.m_nameSid); target_it != target_indices.end()) {
                modules_patch_target& target = targets[target_it->second];
                target.m_oldSize = old_entry.m_size;
                size = target.m_newSize;
                if (!target.m_exports.empty()) {
                    exports = normalized_module_exports(target.m_name, target.m_exports);
                }
            }
            entry_indices[old_entry.m_nameSid] = entries.size();
            entries.push_back(module_entry_source{
                old_entry.m_name,
                old_entry.m_nameSid,
                size,
                copy_modules_symbol_array(old_entry.m_imports),
                std::move(exports),
                0,
            });
        }

        for (modules_patch_target& target : targets) {
            if (entry_indices.contains(target.m_sid)) {
                continue;
            }
            entry_indices[target.m_sid] = entries.size();
            entries.push_back(module_entry_source{
                target.m_name,
                target.m_sid,
                target.m_newSize,
                default_module_imports(),
                normalized_module_exports(target.m_name, target.m_exports),
                0,
            });
        }

        constexpr sid64 modules_sid = SID("*modules*");
        constexpr sid64 array_sid = SID("array");
        constexpr u8 modules_entry_reloc_bits = 0b11001;

        program_binary_element element{entries.size() * sizeof(ModulesEntry)};
        global_state global{};
        element.m_entry = Entry{modules_sid, module_info_array_sid, nullptr};

        element.push_bytes(module_info_array_sid, 0b0);
        const u64 module_array_offset = element.m_rawData.size();
        const ModuleInfoArray new_array{
            static_cast<u32>(entries.size()),
            module_array->m_unk,
            nullptr,
        };
        element.push_bytes(new_array, 0b010);
        element.push_bytes(array_sid, 0b0);
        const u64 entries_offset = element.m_rawData.size();
        patch_element_u64(element, module_array_offset + offsetof(ModuleInfoArray, m_entries), entries_offset);

        for (module_entry_source& source : entries) {
            source.m_rawEntryOffset = element.m_rawData.size();
            const u64 name_index = global.add_string(source.m_name);
            element.insert_string_offset(offsetof(ModulesEntry, m_name));
            const ModulesEntry entry{
                reinterpret_cast<const char*>(name_index),
                source.m_nameSid,
                source.m_size,
                nullptr,
                nullptr,
            };
            element.push_bytes(entry, modules_entry_reloc_bits);
        }

        std::vector<modules_symbol_array_payload> symbol_array_payloads;
        symbol_array_payloads.reserve(entries.size() * 2);
        for (const module_entry_source& source : entries) {
            const u64 imports_offset = emit_modules_symbol_array_struct(element, source.m_imports);
            patch_element_u64(element, source.m_rawEntryOffset + offsetof(ModulesEntry, m_imports), imports_offset);
            symbol_array_payloads.push_back(modules_symbol_array_payload{imports_offset, &source.m_imports});

            const u64 exports_offset = emit_modules_symbol_array_struct(element, source.m_exports);
            patch_element_u64(element, source.m_rawEntryOffset + offsetof(ModulesEntry, m_exports), exports_offset);
            symbol_array_payloads.push_back(modules_symbol_array_payload{exports_offset, &source.m_exports});
        }

        for (const modules_symbol_array_payload& payload : symbol_array_payloads) {
            emit_modules_symbol_array_payload(element, payload);
        }

        std::vector<program_binary_element> elements;
        elements.push_back(std::move(element));
        auto binary = ast::program::make_binary(std::move(elements), global);
        if (!binary) {
            return std::unexpected{binary.error()};
        }

        const auto& [bytes, size] = *binary;
        std::ofstream modules_file{modules, std::ios::binary | std::ios::trunc};
        if (!modules_file.is_open()) {
            return std::unexpected{"couldn't open " + modules.string()};
        }
        modules_file.write(reinterpret_cast<const char*>(bytes.get()), size);

        std::vector<modules_patch_summary> summaries;
        summaries.reserve(targets.size());
        constexpr u64 raw_data_start = sizeof(DC_Header) + sizeof(sid64) + sizeof(Entry);
        for (const modules_patch_target& target : targets) {
            const module_entry_source& source = entries[entry_indices.at(target.m_sid)];
            const u64 size_offset = raw_data_start + source.m_rawEntryOffset + offsetof(ModulesEntry, m_size);
            summaries.push_back(modules_patch_summary{modules, target.m_name, target.m_oldSize, target.m_newSize, size_offset});
        }
        return summaries;
    }

    [[nodiscard]] static resstr<modules_patch_summary> patch_modules_size(
        const std::filesystem::path& modules,
        const std::filesystem::path& output,
        const u64 new_size,
        const std::vector<sid64>& exports = {}
    ) {
        resstr<std::vector<modules_patch_summary>> summaries = patch_modules_sizes(
            modules,
            {modules_patch_request{output, new_size, exports}}
        );
        if (!summaries) {
            return std::unexpected{summaries.error()};
        }
        return summaries->front();
    }

    [[nodiscard]] static errmsg prepare_mod_workspace(const compiler_options& options) {
        std::error_code ec;
        const std::filesystem::path mod_dc1 = options.m_modules.parent_path();
        std::filesystem::create_directories(mod_dc1, ec);
        if (ec) {
            return "couldn't create mod directory " + mod_dc1.string() + ": " + ec.message();
        }
        std::filesystem::create_directories(options.m_output.parent_path(), ec);
        if (ec) {
            return "couldn't create output directory " + options.m_output.parent_path().string() + ": " + ec.message();
        }

        const std::filesystem::path modules_source = game_dc1_dir(options.m_game) / "modules.bin";
        if (!std::filesystem::exists(modules_source)) {
            return "expected a vanilla modules.bin in the game's dc1 directory but got missing path " + modules_source.string();
        }
        std::filesystem::copy_file(modules_source, options.m_modules, std::filesystem::copy_options::overwrite_existing, ec);
        if (ec) {
            return "couldn't copy modules.bin from " + modules_source.string() + " to " + options.m_modules.string() + ": " + ec.message();
        }
        return std::nullopt;
    }

    static resstr<compiler_output_summary> create_output(
        const dconstruct::compilation::compiler_options& filepaths,
        const dconstruct::SIDBase& sidbase,
        const std::vector<dconstruct::compilation::program_binary_element>& functions,
        dconstruct::compilation::global_state& global
    ) {
        const auto binary_res = filepaths.m_standalone ? dconstruct::ast::program::make_binary(functions, global)
                                                       : dconstruct::compilation::disassemble_target(filepaths.m_target, sidbase, functions, global);

        if (!binary_res) {
            return std::unexpected{binary_res.error()};
        }
        const auto& [bytes, size] = *binary_res;
        std::filesystem::create_directories(filepaths.m_output.parent_path());
        std::ofstream of(filepaths.m_output, std::ios::binary);
        if (!of.is_open()) {
            return std::unexpected{"couldn't open filepath " + filepaths.m_output.string()};
        }
        of.write(reinterpret_cast<const char*>(bytes.get()), size);
        of.flush();

        compiler_output_summary summary;
        summary.m_output = filepaths.m_output;
        summary.m_size = size;

        return summary;
    }

    [[nodiscard]] static std::filesystem::path repackage_psarc_path(const std::filesystem::path& directory_path) {
        std::filesystem::path psarc_path = directory_path;
        psarc_path += ".psarc";
        return psarc_path;
    }

    [[nodiscard]] static resstr<std::filesystem::path> repackage_psarc(const std::filesystem::path& directory_path) {
        const std::filesystem::path psarc_path = repackage_psarc_path(directory_path);
        const std::string command = "ndarc -c \"" + directory_path.string() + "\" -o \"" + psarc_path.string() + "\"";
        const int result = std::system(command.c_str());
        if (result != 0) {
            return std::unexpected{"ndarc failed while producing " + psarc_path.string()};
        }
        return psarc_path;
    }

}
