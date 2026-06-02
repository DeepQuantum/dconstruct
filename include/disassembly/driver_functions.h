#pragma once

#include "ast/ast_source.h"
#include "disassembly/disassembler.h"
#include "disassembly/mapping_registry.h"
#include "decompilation/decomp_function.h"
#include "about.h"
#include "cxxopts.hpp"
#include <chrono>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace dconstruct::disassembly {

void decomp_file(
    const std::filesystem::path &inpath, 
    const std::filesystem::path &out_disasm_filename, 
    const std::filesystem::path &out_decomp_filename,
    const dconstruct::SIDBase &base,
    const std::unordered_map<sid64, dconstruct::ast::full_type> &type_map,
    const bool write_graphs,
    const dconstruct::ast::LANGUAGE_FLAGS language_flags,
    const bool show_warnings,
    const dconstruct::dcompiler::OPTIMIZATION_KIND optimizations,
    const std::vector<std::string> &edits = {},
    const dconstruct::game_type game = dconstruct::game_type::T2R,
    const std::vector<std::filesystem::path> &type_map_paths = {});

void disasm_file(
    const std::filesystem::path &inpath,
    const std::filesystem::path &out_filename,
    const dconstruct::SIDBase &base,
    const std::unordered_map<sid64, dconstruct::ast::full_type> &type_map,
    const std::vector<std::string> &edits = {},
    const dconstruct::game_type game = dconstruct::game_type::T2R);

void decompile_multiple(
    const std::filesystem::path &in, 
    const std::filesystem::path &out,
    const dconstruct::SIDBase &sidbase,
    const std::unordered_map<sid64, dconstruct::ast::full_type> &type_map,
    const bool generate_graphs,
    const bool show_warnings,
    const bool optimize,
    const dconstruct::ast::LANGUAGE_FLAGS language_flags,
    const dconstruct::game_type game = dconstruct::game_type::T2R,
    const std::vector<std::filesystem::path> &type_map_paths = {}
);

void disassemble_multiple(
    const std::filesystem::path &in,
    const std::filesystem::path &out,
    const dconstruct::SIDBase &sidbase,
    const std::unordered_map<sid64, dconstruct::ast::full_type> &type_map,
    const dconstruct::game_type game = dconstruct::game_type::T2R
);

void map_types_multiple_to_file(
    const std::filesystem::path &in,
    const dconstruct::SIDBase &sidbase,
    const std::filesystem::path &out_types_file,
    const dconstruct::game_type game = dconstruct::game_type::T2R
);

std::vector<std::string> edits_from_file(const std::filesystem::path &path);

// static i32 disassemble_shader(const std::filesystem::path& path) {
//     const auto ir_exp = dconstruct::shaders::ndshader_file::parse_from_file(path);
//     if (!ir_exp) {
//         std::cerr << "couldn't process shader file: " << ir_exp.error() << "\n";
//         return -1;
//     }
//     const auto [msg, success] = ir_exp->to_string();
//     if (!success) {
//         std::cerr << msg << "\n";
//         return -1;
//     }
//     std::cout << msg << "\n";
//     return 0;
// }

[[nodiscard]] std::optional<dconstruct::ast::LANGUAGE_FLAGS> get_print_type(const std::string& input_string);

[[nodiscard]] std::optional<dconstruct::game_type> get_game_type(const std::string& input_string);

using type_map_t = std::unordered_map<sid64, ast::full_type>;

[[nodiscard]] std::expected<type_map_t, std::string> parse_type_defs_file(const std::filesystem::path& filepath);

// function -> <var, type>

[[nodiscard]] std::expected<std::unordered_map<sid64, compilation::scope>, std::string> parse_var_type_map_file(const std::filesystem::path& filepath, const std::unordered_map<sid64, ast::full_type>* mapped_types = nullptr);

[[nodiscard]] std::optional<std::pair<cxxopts::Options, cxxopts::ParseResult>> get_command_line_options(int argc, char* argv[]);


}
