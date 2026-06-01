#pragma once

#include "ast/ast_source.h"
#include "disassembly/disassembler.h"
#include "disassembly/mapping_registry.h"
#include "about.h"
#include "cxxopts.hpp"
#include <chrono>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace dconstruct::disassembly {

void decomp_file(
    const std::filesystem::path &inpath, 
    const std::filesystem::path &out_disasm_filename, 
    const std::filesystem::path &out_decomp_filename,
    const dconstruct::SIDBase &base,
    const bool write_graphs,
    const dconstruct::ast::LANGUAGE_FLAGS language_flags,
    const bool show_warnings,
    const bool optimize,
    const std::vector<std::string> &edits = {},
    const dconstruct::game_type game = dconstruct::game_type::T2R);

void disasm_file(
    const std::filesystem::path &inpath, 
    const std::filesystem::path &out_filename,
    const dconstruct::SIDBase &base,
    const std::vector<std::string> &edits = {},
    const dconstruct::game_type game = dconstruct::game_type::T2R);

void decompile_multiple(
    const std::filesystem::path &in, 
    const std::filesystem::path &out,
    const dconstruct::SIDBase &sidbase,
    const bool generate_graphs,
    const bool show_warnings,
    const bool optimize,
    const dconstruct::ast::LANGUAGE_FLAGS language_flags,
    const dconstruct::game_type game = dconstruct::game_type::T2R
);

void disassemble_multiple(
    const std::filesystem::path &in, 
    const std::filesystem::path &out,
    const dconstruct::SIDBase &sidbase,
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

[[nodiscard]] std::optional<std::pair<cxxopts::Options, cxxopts::ParseResult>> get_command_line_options(int argc, char* argv[]);

}
