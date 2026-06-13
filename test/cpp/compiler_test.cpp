#include <gtest/gtest.h>
#include "compilation/lexer.h"
#include "compilation/dc_parser.h"
#include "compilation/compiler_funcs.h"
#include "compilation/preprocessor.h"
#include "compilation/function.h"
#include "disassembly/disassembler.h"
#include "disassembly/driver_functions.h"
#include "disassembly/instructions.h"
#include "decompilation/decomp_function.h"
#include "binaryfile.h"
#include "ast/ast.h"
#include <vector>
#include <fstream>
#include <algorithm>
#include <bit>
#include <filesystem>
#include <optional>
#include <regex>
#include <sstream>

namespace dconstruct::testing {

    const std::filesystem::path TEST_ROOT = DCONSTRUCT_TEST_ROOT;
    const std::string TEST_DIR = (TEST_ROOT / "fixtures" / "dc").string() + "/";
    const std::string DCPL_PATH = (TEST_ROOT / "fixtures" / "dcpl").string() + "/";

    static SIDBase base = *SIDBase::from_binary(TEST_DIR + R"(\test_sidbase.bin)");

    static std::pair<std::vector<compilation::token>, std::vector<compilation::lexing_error>> get_tokens(const std::string& string) {
        dconstruct::compilation::Lexer lexer = dconstruct::compilation::Lexer(string);
        return {lexer.scan_tokens(), lexer.get_errors()};
    }

    static cxxopts::ParseResult get_empty_options() {
        cxxopts::Options options("compiler_test", "");
        const char* argv[] = {"compiler_test"};
        return options.parse(1, argv);
    }

    static std::string dcpl_prelude() {
        std::filesystem::create_directories("test/fixtures/compiler/include_game");
        return "@game \"test/fixtures/compiler/include_game\"\n@mod \"include_mod\"\n";
    }

    static std::tuple<ast::program, std::unordered_map<std::string, ast::full_type>, std::vector<compilation::parsing_error>> get_parse_results(const std::vector<compilation::token>& tokens) {
        compilation::Parser parser{tokens};
        return {parser.parse(), parser.get_known_types(), parser.get_errors()};
    }

    static std::pair<std::list<stmnt_uptr>, std::vector<compilation::parsing_error>> get_statements(std::vector<compilation::token>& tokens) {
        if (tokens.back().m_type == compilation::token_type::_EOF) {
            tokens.pop_back();
        }

        std::vector<compilation::token> function_def_tokens{
            compilation::token(compilation::token_type::IDENTIFIER, "i32", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "main", 0, 1),
            compilation::token(compilation::token_type::LEFT_PAREN, "(", 0, 1),
            compilation::token(compilation::token_type::RIGHT_PAREN, ")", 0, 1),
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 1),
        };

        function_def_tokens.insert(function_def_tokens.end(), tokens.begin(), tokens.end());

        function_def_tokens.push_back(compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 1));
        function_def_tokens.push_back(compilation::token(compilation::token_type::_EOF, "", 0, 1));

        compilation::Parser parser{function_def_tokens};

        auto [program, _, errors] = get_parse_results(function_def_tokens);

        return {!program.m_declarations.empty() ? std::move(static_cast<ast::function_definition*>(program.m_declarations[0].get())->m_body.m_statements) : std::list<stmnt_uptr>{}, errors};
    }

    [[nodiscard]] static resstr<std::vector<compilation::function_context>> compile_to_functions(const std::string& code) {
        const auto [tokens, lex_errors] = get_tokens(code);
        if (!lex_errors.empty()) {
            return std::unexpected{"lex errors: " + lex_errors.front().m_message};
        }
        auto [program, types, parse_errors] = get_parse_results(tokens);
        if (!parse_errors.empty()) {
            return std::unexpected{"parse errors: " + parse_errors.front().m_message};
        }
        compilation::scope scope{types};
        compilation::add_global_functions(scope);
        const auto semantic_errors = program.check_semantics(scope);
        if (!semantic_errors.empty()) {
            return std::unexpected{"semantic errors: " + semantic_errors.front().m_message};
        }
        compilation::global_state global{};
        std::vector<compilation::function_context> functions;
        for (const auto& decl : program.m_declarations) {
            auto* func_def = dynamic_cast<const ast::function_definition*>(decl.get());
            if (!func_def) {
                continue;
            }
            compilation::function_context fn{};
            if (std::holds_alternative<std::string>(func_def->m_name)) {
                fn.m_name = std::get<std::string>(func_def->m_name);
            }
            for (u32 i = 0; i < func_def->m_parameters.size(); ++i) {
                const auto& param = func_def->m_parameters[i];
                const auto new_var_reg = fn.get_next_unused_register();
                if (!new_var_reg) {
                    return std::unexpected{new_var_reg.error()};
                }
                fn.m_varsToRegs.define(param.m_name, *new_var_reg);
                fn.emit_instruction(Opcode::Move, *new_var_reg, ARGUMENT_REGISTERS_IDX + i);
            }
            const auto body_err = func_def->m_body.emit_dc(fn, global);
            if (body_err) {
                return std::unexpected{*body_err};
            }
            if (!fn.m_returnBranchLocations.empty() && fn.m_returnBranchLocations.back() == fn.m_instructions.size() - 1) {
                fn.m_instructions.back() = Instruction(Opcode::Return, 0_r, 0_r);
                fn.m_returnBranchLocations.pop_back();
            } else {
                fn.emit_instruction(Opcode::Return, 0_r, 0_r);
            }
            const u16 return_location = fn.m_instructions.size() - 1;
            for (const u64 branch_location : fn.m_returnBranchLocations) {
                Instruction& branch = fn.m_instructions[branch_location];
                branch.set_lo_hi(return_location);
            }
            functions.push_back(std::move(fn));
        }
        return functions;
    }

    static void expect_instructions(const std::string& code, const std::vector<Instruction>& expected) {
        auto functions = compile_to_functions(code);
        ASSERT_TRUE(functions) << "compile failed: " << (functions ? "" : functions.error());
        ASSERT_FALSE(functions->empty()) << "no functions produced";
        EXPECT_EQ((*functions)[0].m_instructions, expected);
    }

    TEST(COMPILER, LexerEmpty) {
        const std::string empty = "";
        const auto [tokens, errors] = get_tokens(empty);
        const compilation::token eof = compilation::token(compilation::token_type::_EOF, "", 0, 1);

        EXPECT_EQ(tokens.back(), eof);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, WrongStatementCast) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 1),
            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)};

        const auto [statements, errors] = get_statements(tokens);

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);

        const ast::variable_declaration* actual = dynamic_cast<const ast::variable_declaration*>(statements.front().get());
        EXPECT_EQ(actual, nullptr);
    }

    TEST(COMPILER, WrongExpressionCast) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 1),
            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)};

        const auto [statements, errors] = get_statements(tokens);

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        const ast::block* block = static_cast<const ast::block*>(statements.front().get());
        const ast::expression_stmt* expr = static_cast<const ast::expression_stmt*>(block->m_statements.front().get());
        const ast::binary_expr* bin_expr = dynamic_cast<const ast::binary_expr*>(expr->m_expression.get());

        EXPECT_EQ(bin_expr, nullptr);
    }

    TEST(COMPILER, LexerBasicChars) {
        const std::string chars = "+.{)$";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            compilation::token(compilation::token_type::DOT, ".", 0, 1),
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 1),
            compilation::token(compilation::token_type::RIGHT_PAREN, ")", 0, 1),
            compilation::token(compilation::token_type::DOLLAR, "$", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerDoubleChars) {
        const std::string chars = "!=,==}<==";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::BANG_EQUAL, "!=", 0, 1),
            compilation::token(compilation::token_type::COMMA, ",", 0, 1),
            compilation::token(compilation::token_type::EQUAL_EQUAL, "==", 0, 1),
            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 1),
            compilation::token(compilation::token_type::LESS_EQUAL, "<=", 0, 1),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerInvalidCharacter) {
        const std::string chars = "@";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };

        const std::vector<compilation::lexing_error> expected_errors = {
            compilation::lexing_error(1, "invalid token '@'")};

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors, expected_errors);
    }

    TEST(COMPILER, IncludeExpandsRecursivelyAndOnlyOnce) {
        std::string source = dcpl_prelude() +
                             "@include \"include_a.dcpl\"\n"
                             "@include \"include_a.dcpl\"\n"
                             "u32 main() { return included_b(); }\n";
        std::vector<compilation::source_location> line_map;

        auto options = compilation::compiler_options::parse(get_empty_options(), source, "test/fixtures/compiler/include_main.dcpl", line_map);
        ASSERT_TRUE(options) << options.error();

        compilation::Lexer lexer{source, &line_map};
        const auto [tokens, lex_errors] = lexer.get_results();
        ASSERT_TRUE(lex_errors.empty());

        const auto count_identifier = [&](const std::string& name) {
            return std::ranges::count_if(
                tokens,
                [&](const compilation::token& token) {
                    return token.m_type == compilation::token_type::IDENTIFIER && token.m_lexeme == name;
                }
            );
        };

        EXPECT_EQ(count_identifier("included_a"), 1);
        EXPECT_EQ(count_identifier("included_b"), 2);
    }

    TEST(COMPILER, IncludeMapsErrorsToIncludedFile) {
        std::string source = dcpl_prelude() +
                             "@include \"include_error.dcpl\"\n"
                             "u32 main() {\n"
                             "    return 0;\n"
                             "}\n";
        std::vector<compilation::source_location> line_map;

        auto options = compilation::compiler_options::parse(get_empty_options(), source, "test/fixtures/compiler/include_main.dcpl", line_map);
        ASSERT_TRUE(options) << options.error();

        compilation::Lexer lexer{source, &line_map};
        const auto [tokens, lex_errors] = lexer.get_results();
        ASSERT_TRUE(lex_errors.empty());

        auto [program, types, parse_errors] = get_parse_results(tokens);
        ASSERT_FALSE(parse_errors.empty());
        EXPECT_EQ(parse_errors.front().m_token.m_file.filename(), "include_error.dcpl");
        EXPECT_EQ(parse_errors.front().m_token.m_line, 3);
    }

    TEST(COMPILER, IncludeDoesNotShiftOriginalFileErrors) {
        std::string source = dcpl_prelude() +
                             "@include \"include_a.dcpl\"\n"
                             "u32 main() {\n"
                             "    return 0\n"
                             "}\n";
        std::vector<compilation::source_location> line_map;

        auto options = compilation::compiler_options::parse(get_empty_options(), source, "test/fixtures/compiler/include_main_error.dcpl", line_map);
        ASSERT_TRUE(options) << options.error();

        compilation::Lexer lexer{source, &line_map};
        const auto [tokens, lex_errors] = lexer.get_results();
        ASSERT_TRUE(lex_errors.empty());

        auto [program, types, parse_errors] = get_parse_results(tokens);
        ASSERT_FALSE(parse_errors.empty());
        EXPECT_EQ(parse_errors.front().m_token.m_file.filename(), "include_main_error.dcpl");
        EXPECT_EQ(parse_errors.front().m_token.m_line, 6);
    }

    TEST(COMPILER, AddPakParsesAndAppliesMultilineEntries) {
        const std::filesystem::path game_path = "test/fixtures/compiler/pak68_macro_game";
        const std::filesystem::path mod_path = game_path / "mods" / "pak68_macro_mod";
        const std::filesystem::path pak_path = mod_path / "pak68_macro_mod-pak68.txt";
        std::filesystem::create_directories(mod_path);
        {
            std::ofstream out{pak_path};
            out << "level-name sp-all\n"
                << "symbol existing-entry\n"
                << "level-name other-level\n"
                << "actor other-actor\n";
        }

        std::string source =
            "@game \"test/fixtures/compiler/pak68_macro_game\"\n"
            "@mod \"pak68_macro_mod\"\n"
            "@add_pak level-name sp-all {\n"
            "  symbol #gas-mask-ellie\n"
            "  actor dina\n"
            "}\n"
            "u32 main() { return 0; }\n";
        std::vector<compilation::source_location> line_map;

        auto options = compilation::compiler_options::parse(get_empty_options(), source, "test/fixtures/compiler/pak68_macro_test.dcpl", line_map);
        ASSERT_TRUE(options) << options.error();
        ASSERT_TRUE(options->m_pak68);
        EXPECT_EQ(*options->m_pak68, pak_path);
        ASSERT_EQ(options->m_pak68Edits.size(), 1);
        EXPECT_EQ(options->m_pak68Edits[0].m_categoryType, "level-name");
        EXPECT_EQ(options->m_pak68Edits[0].m_levelName, "sp-all");
        ASSERT_EQ(options->m_pak68Edits[0].m_entries.size(), 2);
        EXPECT_EQ(options->m_pak68Edits[0].m_entries[0], (compilation::pak68_entry{compilation::pak68_type::SYMBOL, "gas-mask-ellie"}));
        EXPECT_EQ(options->m_pak68Edits[0].m_entries[1], (compilation::pak68_entry{compilation::pak68_type::ACTOR, "dina"}));
        EXPECT_EQ(source.find("@add_pak"), std::string::npos);

        auto apply_res = compilation::apply_pak68_edits(*options->m_pak68, options->m_pak68Edits);
        ASSERT_TRUE(apply_res) << apply_res.error();
        ASSERT_EQ(apply_res->size(), 1);
        EXPECT_EQ((*apply_res)[0].m_added.size(), 2);

        std::ifstream in{pak_path};
        ASSERT_TRUE(in.is_open());
        std::stringstream contents;
        contents << in.rdbuf();
        EXPECT_NE(contents.str().find("symbol gas-mask-ellie\nactor dina\nlevel-name other-level"), std::string::npos);
    }

    TEST(COMPILER, AddPakRejectsUnknownType) {
        const std::filesystem::path game_path = "test/fixtures/compiler/pak68_unknown_type_game";
        std::filesystem::create_directories(game_path / "mods" / "pak68_unknown_type_mod");

        std::string source =
            "@game \"test/fixtures/compiler/pak68_unknown_type_game\"\n"
            "@mod \"pak68_unknown_type_mod\"\n"
            "@add_pak level-name sp-all { nope #gas-mask-ellie }\n"
            "u32 main() { return 0; }\n";
        std::vector<compilation::source_location> line_map;

        auto options = compilation::compiler_options::parse(get_empty_options(), source, "test/fixtures/compiler/pak68_unknown_type_test.dcpl", line_map);
        ASSERT_FALSE(options);
        EXPECT_NE(options.error().find("unknown pak68 type: nope"), std::string::npos);
    }

    TEST(COMPILER, AddPakCreatesMissingLevel) {
        const std::filesystem::path game_path = "test/fixtures/compiler/pak68_missing_level_game";
        const std::filesystem::path mod_path = game_path / "mods" / "pak68_missing_level_mod";
        const std::filesystem::path pak_path = mod_path / "pak68_missing_level_mod-pak68.txt";
        std::filesystem::create_directories(mod_path);
        std::filesystem::remove(pak_path);

        std::string source =
            "@game \"test/fixtures/compiler/pak68_missing_level_game\"\n"
            "@mod \"pak68_missing_level_mod\"\n"
            "@add_pak level-name missing-level { symbol #gas-mask-ellie }\n"
            "u32 main() { return 0; }\n";
        std::vector<compilation::source_location> line_map;

        auto options = compilation::compiler_options::parse(get_empty_options(), source, "test/fixtures/compiler/pak68_missing_level_test.dcpl", line_map);
        ASSERT_TRUE(options) << options.error();

        auto apply_res = compilation::apply_pak68_edits(*options->m_pak68, options->m_pak68Edits);
        ASSERT_TRUE(apply_res) << apply_res.error();
        ASSERT_EQ(apply_res->size(), 1);
        EXPECT_EQ((*apply_res)[0].m_levelName, "missing-level");
        ASSERT_EQ((*apply_res)[0].m_added.size(), 1);
        EXPECT_EQ((*apply_res)[0].m_added[0], (compilation::pak68_entry{compilation::pak68_type::SYMBOL, "gas-mask-ellie"}));

        std::ifstream in{pak_path};
        ASSERT_TRUE(in.is_open());
        std::stringstream contents;
        contents << in.rdbuf();
        EXPECT_NE(contents.str().find("level-name missing-level\nsymbol gas-mask-ellie\n"), std::string::npos);
    }

    TEST(COMPILER, ModDerivesStandaloneOutputModulesPakAndRepackage) {
        const std::filesystem::path game_path = "test/fixtures/compiler/derived_game";
        const std::filesystem::path mod_path = game_path / "mods" / "derived_mod";
        std::filesystem::create_directories(game_path / "mods");

        std::string source =
            "@game \"test/fixtures/compiler/derived_game\"\n"
            "@mod \"derived_mod\"\n"
            "u32 main() { return 0; }\n";
        std::vector<compilation::source_location> line_map;

        auto options = compilation::compiler_options::parse(get_empty_options(), source, "test/fixtures/compiler/derived_mod_test.dcpl", line_map);
        ASSERT_TRUE(options) << options.error();
        EXPECT_TRUE(options->m_standalone);
        EXPECT_TRUE(options->m_target.empty());
        EXPECT_EQ(options->m_game, game_path);
        EXPECT_EQ(options->m_output, mod_path / "bin" / "dc1" / "derived_mod.bin");
        EXPECT_EQ(options->m_modules, mod_path / "bin" / "dc1" / "modules.bin");
        ASSERT_TRUE(options->m_pak68);
        EXPECT_EQ(*options->m_pak68, mod_path / "derived_mod-pak68.txt");
        ASSERT_TRUE(options->m_repackage);
        EXPECT_EQ(*options->m_repackage, mod_path);
    }

    TEST(COMPILER, RepackagePathUsesExactModFolderName) {
        EXPECT_EQ(
            compilation::repackage_psarc_path("test/fixtures/compiler/derived_mod"),
            std::filesystem::path{"test/fixtures/compiler/derived_mod.psarc"}
        );
        EXPECT_EQ(
            compilation::repackage_psarc_path("test/fixtures/compiler/derived_mod_unpacked"),
            std::filesystem::path{"test/fixtures/compiler/derived_mod_unpacked.psarc"}
        );
    }

    TEST(COMPILER, TargetModeResolvesAgainstGameDc1) {
        const std::filesystem::path game_path = "test/fixtures/compiler/target_mode_game";
        const std::filesystem::path game_dc1 = game_path / "build" / "pc" / "main" / "bin_unpacked" / "dc1";
        std::filesystem::create_directories(game_dc1 / "rogue");
        std::filesystem::create_directories(game_path / "mods");
        {
            std::ofstream target{game_dc1 / "rogue" / "script-callbacks.bin", std::ios::binary};
            target << "placeholder";
        }

        std::string source =
            "@game \"test/fixtures/compiler/target_mode_game\"\n"
            "@mod \"target_mode_mod\"\n"
            "@target \"rogue/script-callbacks\"\n"
            "u32 main() { return 0; }\n";
        std::vector<compilation::source_location> line_map;

        auto options = compilation::compiler_options::parse(get_empty_options(), source, "test/fixtures/compiler/target_mode_test.dcpl", line_map);
        ASSERT_TRUE(options) << options.error();
        EXPECT_FALSE(options->m_standalone);
        EXPECT_EQ(options->m_target, game_dc1 / "rogue" / "script-callbacks.bin");
        EXPECT_EQ(options->m_output, game_path / "mods" / "target_mode_mod" / "bin" / "dc1" / "rogue" / "script-callbacks.bin");
        EXPECT_EQ(options->m_modules, game_path / "mods" / "target_mode_mod" / "bin" / "dc1" / "modules.bin");
    }

    TEST(COMPILER, MissingGameDirectiveFails) {
        std::string source =
            "@mod \"no_game_mod\"\n"
            "u32 main() { return 0; }\n";
        std::vector<compilation::source_location> line_map;

        auto options = compilation::compiler_options::parse(get_empty_options(), source, "test/fixtures/compiler/no_game_test.dcpl", line_map);
        ASSERT_FALSE(options);
        EXPECT_NE(options.error().find("@game"), std::string::npos);
    }

    TEST(COMPILER, MissingTargetFileFails) {
        const std::filesystem::path game_path = "test/fixtures/compiler/missing_target_game";
        std::filesystem::create_directories(game_path / "build" / "pc" / "main" / "bin_unpacked" / "dc1");

        std::string source =
            "@game \"test/fixtures/compiler/missing_target_game\"\n"
            "@mod \"missing_target_mod\"\n"
            "@target \"rogue/does-not-exist\"\n"
            "u32 main() { return 0; }\n";
        std::vector<compilation::source_location> line_map;

        auto options = compilation::compiler_options::parse(get_empty_options(), source, "test/fixtures/compiler/missing_target_test.dcpl", line_map);
        ASSERT_FALSE(options);
        EXPECT_NE(options.error().find("missing path"), std::string::npos);
    }

    TEST(COMPILER, PrepareModWorkspaceCopiesFreshModules) {
        const std::filesystem::path game_path = "test/fixtures/compiler/prepare_game";
        const std::filesystem::path game_dc1 = game_path / "build" / "pc" / "main" / "bin_unpacked" / "dc1";
        const std::filesystem::path mod_dc1 = game_path / "mods" / "prepare_mod" / "bin" / "dc1";
        std::filesystem::create_directories(game_dc1);
        std::filesystem::create_directories(game_path / "mods");
        std::filesystem::remove_all(game_path / "mods" / "prepare_mod");
        std::filesystem::copy_file(
            TEST_DIR + "sidbase_new_fixed.bin",
            game_dc1 / "modules.bin",
            std::filesystem::copy_options::overwrite_existing
        );

        std::string source =
            "@game \"test/fixtures/compiler/prepare_game\"\n"
            "@mod \"prepare_mod\"\n"
            "u32 main() { return 0; }\n";
        std::vector<compilation::source_location> line_map;

        auto options = compilation::compiler_options::parse(get_empty_options(), source, "test/fixtures/compiler/prepare_test.dcpl", line_map);
        ASSERT_TRUE(options) << options.error();

        const dconstruct::errmsg err = compilation::prepare_mod_workspace(*options);
        ASSERT_FALSE(err) << *err;
        EXPECT_TRUE(std::filesystem::exists(mod_dc1 / "modules.bin"));
        EXPECT_EQ(
            std::filesystem::file_size(mod_dc1 / "modules.bin"),
            std::filesystem::file_size(game_dc1 / "modules.bin")
        );
    }

    TEST(COMPILER, PatchModulesSizeHandlesMixedPathSeparators) {
        const std::filesystem::path modules_path = "test/fixtures/compiler/modules_mixed_separators.bin";
        std::error_code cleanup_ec;
        std::filesystem::remove(modules_path, cleanup_ec);
        std::filesystem::copy_file("test/fixtures/bin/modules.bin", modules_path, std::filesystem::copy_options::overwrite_existing);

        constexpr sid64 target_sid = SID("ss-rogue/test-script-qntm");
        constexpr u64 new_size = 0x5678;

        const std::filesystem::path output_path =
            R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/mods/ProjectFEDRA_unpacked\bin\dc1\ss-rogue/test-script-qntm.bin)";
        auto patch = compilation::patch_modules_size(modules_path, output_path, new_size);
        ASSERT_TRUE(patch) << patch.error();
        EXPECT_EQ(patch->m_targetName, "ss-rogue/test-script-qntm");
        EXPECT_EQ(patch->m_oldSize, 691);
        EXPECT_EQ(patch->m_newSize, new_size);

        auto patched_file = BinaryFile::from_path(modules_path);
        ASSERT_TRUE(patched_file) << patched_file.error();
        const auto* module_array = reinterpret_cast<const compilation::ModuleInfoArray*>(patched_file->m_dcheader->m_pStartOfData->m_entryPtr);
        bool found = false;
        for (u32 i = 0; i < module_array->m_numEntries; ++i) {
            const ModulesEntry& entry = module_array->m_entries[i];
            if (entry.m_nameSid == target_sid) {
                found = true;
                EXPECT_EQ(entry.m_size, new_size);
            }
        }
        EXPECT_TRUE(found);

        std::filesystem::remove(modules_path, cleanup_ec);
    }

    TEST(COMPILER, PatchModulesSizeRefreshesExistingExports) {
        const std::filesystem::path modules_path = "test/fixtures/compiler/modules_refresh_exports.bin";
        std::error_code cleanup_ec;
        std::filesystem::remove(modules_path, cleanup_ec);
        std::filesystem::copy_file("test/fixtures/bin/modules.bin", modules_path, std::filesystem::copy_options::overwrite_existing);

        constexpr sid64 target_sid = SID("ss-rogue/test-script-qntm");
        constexpr u64 new_size = 0x7654;
        const std::vector<sid64> exports{
            SID("helper-before-state-script"),
            SID("test-script-qntm"),
        };

        const std::filesystem::path output_path =
            R"(C:/Program Files (x86)/Steam\steamapps\common\The Last of Us Part II/mods/ProjectFEDRA_unpacked/bin/dc1/ss-rogue/test-script-qntm.bin)";
        auto patch = compilation::patch_modules_size(modules_path, output_path, new_size, exports);
        ASSERT_TRUE(patch) << patch.error();
        EXPECT_EQ(patch->m_targetName, "ss-rogue/test-script-qntm");
        EXPECT_EQ(patch->m_newSize, new_size);

        auto patched_file = BinaryFile::from_path(modules_path);
        ASSERT_TRUE(patched_file) << patched_file.error();
        const auto* module_array = reinterpret_cast<const compilation::ModuleInfoArray*>(patched_file->m_dcheader->m_pStartOfData->m_entryPtr);
        bool found = false;
        for (u32 i = 0; i < module_array->m_numEntries; ++i) {
            const ModulesEntry& entry = module_array->m_entries[i];
            if (entry.m_nameSid == target_sid) {
                found = true;
                EXPECT_EQ(entry.m_size, new_size);
                ASSERT_NE(entry.m_exports, nullptr);
                ASSERT_EQ(entry.m_exports->m_numEntries, exports.size());
                for (u32 j = 0; j < entry.m_exports->m_numEntries; ++j) {
                    EXPECT_EQ(entry.m_exports->m_pSymbols[j], exports[j]);
                }
            }
        }
        EXPECT_TRUE(found);

        std::filesystem::remove(modules_path, cleanup_ec);
    }

    TEST(COMPILER, PatchModulesSizeCreatesMissingEntry) {
        const std::filesystem::path modules_path = "test/fixtures/compiler/modules_missing_entry.bin";
        std::error_code cleanup_ec;
        std::filesystem::remove(modules_path, cleanup_ec);
        std::filesystem::copy_file("test/fixtures/bin/modules.bin", modules_path, std::filesystem::copy_options::overwrite_existing);

        constexpr u64 new_size = 0x9876;
        const std::string target_name = "ss-rogue/generated-module-test";
        const sid64 target_sid = SID(target_name.c_str());
        const std::vector<sid64> exports{
            SID("generated-helper"),
            SID("generated-module-test"),
        };
        const std::filesystem::path output_path =
            R"(C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/mods/ProjectFEDRA_unpacked\bin\dc1\ss-rogue/generated-module-test.bin)";

        const auto before_file = BinaryFile::from_path(modules_path);
        ASSERT_TRUE(before_file) << before_file.error();
        const auto* before_array = reinterpret_cast<const compilation::ModuleInfoArray*>(before_file->m_dcheader->m_pStartOfData->m_entryPtr);
        const u32 old_count = before_array->m_numEntries;

        auto patch = compilation::patch_modules_size(modules_path, output_path, new_size, exports);
        ASSERT_TRUE(patch) << patch.error();
        EXPECT_EQ(patch->m_targetName, target_name);
        EXPECT_EQ(patch->m_oldSize, 0);
        EXPECT_EQ(patch->m_newSize, new_size);

        auto patched_file = BinaryFile::from_path(modules_path);
        ASSERT_TRUE(patched_file) << patched_file.error();
        const auto* module_array = reinterpret_cast<const compilation::ModuleInfoArray*>(patched_file->m_dcheader->m_pStartOfData->m_entryPtr);
        EXPECT_EQ(module_array->m_numEntries, old_count + 1);
        EXPECT_EQ(
            reinterpret_cast<p64>(module_array->m_entries) - reinterpret_cast<p64>(module_array),
            sizeof(compilation::ModuleInfoArray) + sizeof(sid64)
        );
        bool found = false;
        for (u32 i = 0; i < module_array->m_numEntries; ++i) {
            const ModulesEntry& entry = module_array->m_entries[i];
            if (entry.m_nameSid == target_sid) {
                found = true;
                EXPECT_STREQ(entry.m_name, target_name.c_str());
                EXPECT_EQ(entry.m_size, new_size);
                ASSERT_NE(entry.m_imports, nullptr);
                ASSERT_NE(entry.m_exports, nullptr);
                EXPECT_EQ(entry.m_imports->m_numEntries, 10);
                ASSERT_EQ(entry.m_exports->m_numEntries, exports.size());
                for (u32 j = 0; j < entry.m_exports->m_numEntries; ++j) {
                    EXPECT_EQ(entry.m_exports->m_pSymbols[j], exports[j]);
                }
                const p64 imports_struct = reinterpret_cast<p64>(entry.m_imports);
                const p64 exports_struct = reinterpret_cast<p64>(entry.m_exports);
                EXPECT_EQ(exports_struct - imports_struct, sizeof(sid64) + sizeof(SymbolArray));
                EXPECT_GT(reinterpret_cast<p64>(entry.m_imports->m_pSymbols), exports_struct + sizeof(SymbolArray));
                EXPECT_GT(reinterpret_cast<p64>(entry.m_exports->m_pSymbols), exports_struct + sizeof(SymbolArray));
            }
        }
        EXPECT_TRUE(found);

        std::filesystem::remove(modules_path, cleanup_ec);
    }

    TEST(COMPILER, StateScriptsAddAutomaticSpAllPakEntry) {
        compilation::compiler_options options;
        std::vector<std::string> state_scripts{"ss-test-script"};

        const auto edits = compilation::pak68_edits_for_compile(options, state_scripts);
        ASSERT_EQ(edits.size(), 1);
        EXPECT_EQ(edits[0].m_categoryType, "level-name");
        EXPECT_EQ(edits[0].m_levelName, "sp-all");
        ASSERT_EQ(edits[0].m_entries.size(), 1);
        EXPECT_EQ(edits[0].m_entries[0], (compilation::pak68_entry{compilation::pak68_type::SYMBOL, "ss-test-script"}));
    }

    TEST(COMPILER, ExplicitAndAutomaticSpAllPakEntriesShareSection) {
        const std::filesystem::path mod_path = "test/fixtures/compiler/pak68_combined_mod";
        const std::filesystem::path pak_path = mod_path / "pak68.txt";
        std::filesystem::create_directories(mod_path);
        {
            std::ofstream out{pak_path};
            out << "level-name sp-all\n";
        }

        compilation::compiler_options options;
        options.m_pak68Edits.push_back(compilation::pak68_edit_request{
            "level-name",
            "sp-all",
            {compilation::pak68_entry{compilation::pak68_type::ACTOR, "gas-mask-ellie"}},
            {}});

        const auto edits = compilation::pak68_edits_for_compile(options, {"gas-mask-toggle"});
        auto apply_res = compilation::apply_pak68_edits(pak_path, edits);
        ASSERT_TRUE(apply_res) << apply_res.error();
        ASSERT_EQ(apply_res->size(), 1);
        ASSERT_EQ((*apply_res)[0].m_added.size(), 2);

        std::ifstream in{pak_path};
        ASSERT_TRUE(in.is_open());
        std::stringstream contents;
        contents << in.rdbuf();
        EXPECT_EQ(contents.str(), "level-name sp-all\nactor gas-mask-ellie\nsymbol gas-mask-toggle\n");
    }

    TEST(COMPILER, LexerComment) {
        const std::string chars = "/=.//daaindasd}\n+";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::SLASH_EQUAL, "/=", 0, 1),
            compilation::token(compilation::token_type::DOT, ".", 0, 1),
            compilation::token(compilation::token_type::PLUS, "+", 0, 2),
            compilation::token(compilation::token_type::_EOF, "", 0, 2),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerString) {
        const std::string chars = "++\"blackeyeGalaxy_abc\"++";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::PLUS_PLUS, "++", 0, 1),
            compilation::token(compilation::token_type::STRING, "\"blackeyeGalaxy_abc\"", "blackeyeGalaxy_abc", 1),
            compilation::token(compilation::token_type::PLUS_PLUS, "++", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerStringEOF) {
        const std::string chars = "\"blackeyeGalaxy_abc";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };

        const std::vector<compilation::lexing_error> expected_errors = {
            compilation::lexing_error(1, "expected '\"' to close string literal but got end of file")};

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(expected_errors, errors);
    }

    TEST(COMPILER, LexerSimpleInt) {
        const std::string chars = "]{123\n34";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::RIGHT_SQUARE, "]", 0, 1),
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 1),
            compilation::token(compilation::token_type::INT, "123", static_cast<u16>(123), 1),
            compilation::token(compilation::token_type::INT, "34", static_cast<u16>(34), 2),
            compilation::token(compilation::token_type::_EOF, "", 0, 2),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerSimpleDouble) {
        const std::string chars = "123.45\"a\"";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::DOUBLE, "123.45", 123.45f, 1),
            compilation::token(compilation::token_type::STRING, "\"a\"", "a", 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerSimpleHex) {
        const std::string chars = "0x123.abc";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::INT, "0x123", (u16)0x123, 1),
            compilation::token(compilation::token_type::DOT, ".", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "abc", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerSimpleSID) {
        const std::string chars = "1.3-=#ellie";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::DOUBLE, "1.3", 1.3f, 1),
            compilation::token(compilation::token_type::MINUS_EQUAL, "-=", 0, 1),
            compilation::token(compilation::token_type::SID, "#ellie", SID("ellie"), 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerSimpleIdentifier) {
        const std::string chars = "abc\ndef;";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::IDENTIFIER, "abc", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "def", 0, 2),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 2),
            compilation::token(compilation::token_type::_EOF, "", 0, 2),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerSimpleKeywords) {
        const std::string chars = "return;";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::RETURN, "return", 0, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerProgram1) {
        const std::string chars =
            "int main()\n"
            "{"
            "\tint a = 0;"
            "\ta += 1;"
            "\treturn a;"
            "}";

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::IDENTIFIER, "int", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "main", 0, 1),
            compilation::token(compilation::token_type::LEFT_PAREN, "(", 0, 1),
            compilation::token(compilation::token_type::RIGHT_PAREN, ")", 0, 1),
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 2),
            compilation::token(compilation::token_type::IDENTIFIER, "int", 0, 2),
            compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 2),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 2),
            compilation::token(compilation::token_type::INT, "0", (u16)0, 2),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 2),
            compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 2),
            compilation::token(compilation::token_type::PLUS_EQUAL, "+=", 0, 2),
            compilation::token(compilation::token_type::INT, "1", (u16)1, 2),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 2),
            compilation::token(compilation::token_type::RETURN, "return", 0, 2),
            compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 2),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 2),
            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 2),
            compilation::token(compilation::token_type::_EOF, "", 0, 2),
        };

        const auto [tokens, errors] = get_tokens(chars);
        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerComplexSids) {
        const std::string chars = "#simple_sid #%alloc-array? #=f(test123)";
        const auto [tokens, errors] = get_tokens(chars);
        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::SID, "#simple_sid", SID("simple_sid"), 1),
            compilation::token(compilation::token_type::SID, "#%alloc-array?", SID("%alloc-array?"), 1),
            compilation::token(compilation::token_type::SID, "#=f", SID("=f"), 1),
            compilation::token(compilation::token_type::LEFT_PAREN, "(", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "test123", 0, 1),
            compilation::token(compilation::token_type::RIGHT_PAREN, ")", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };

        ASSERT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerProgramWithExtendedFeatures) {
        const std::string chars =
            "struct Person {\n"
            "\tsid name = #ellie;\n"
            "\tdouble damage = 5.9;\n"
            "\tint hexVal = 0x1A3F;\n"
            "\tstring msg = \"Hello, world!\";\n"
            "\tif (damage > 6.0) {\n"
            "\t\tmsg = \"Joel Miller\";\n"
            "\t} else {\n"
            "\t\tmsg = \"Test String\";\n"
            "\t}\n"
            "};";

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::STRUCT, "struct", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "Person", 0, 1),
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 1),

            compilation::token(compilation::token_type::IDENTIFIER, "sid", 0, 2),
            compilation::token(compilation::token_type::IDENTIFIER, "name", 0, 2),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 2),
            compilation::token(compilation::token_type::SID, "#ellie", SID("ellie"), 2),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 2),

            compilation::token(compilation::token_type::IDENTIFIER, "double", 0, 3),
            compilation::token(compilation::token_type::IDENTIFIER, "damage", 0, 3),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 3),
            compilation::token(compilation::token_type::DOUBLE, "5.9", 5.9f, 3),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 3),

            compilation::token(compilation::token_type::IDENTIFIER, "int", 0, 4),
            compilation::token(compilation::token_type::IDENTIFIER, "hexVal", 0, 4),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 4),
            compilation::token(compilation::token_type::INT, "0x1A3F", (u16)0x1A3F, 4),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 4),

            compilation::token(compilation::token_type::IDENTIFIER, "string", 0, 5),
            compilation::token(compilation::token_type::IDENTIFIER, "msg", 0, 5),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 5),
            compilation::token(compilation::token_type::STRING, "\"Hello, world!\"", "Hello, world!", 5),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 5),

            compilation::token(compilation::token_type::IF, "if", 0, 6),
            compilation::token(compilation::token_type::LEFT_PAREN, "(", 0, 6),
            compilation::token(compilation::token_type::IDENTIFIER, "damage", 0, 6),
            compilation::token(compilation::token_type::GREATER, ">", 0, 6),
            compilation::token(compilation::token_type::DOUBLE, "6.0", 6.0f, 6),
            compilation::token(compilation::token_type::RIGHT_PAREN, ")", 0, 6),
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 6),

            compilation::token(compilation::token_type::IDENTIFIER, "msg", 0, 7),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 7),
            compilation::token(compilation::token_type::STRING, "\"Joel Miller\"", "Joel Miller", 7),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 7),

            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 8),
            compilation::token(compilation::token_type::ELSE, "else", 0, 8),
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 8),

            compilation::token(compilation::token_type::IDENTIFIER, "msg", 0, 9),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 9),
            compilation::token(compilation::token_type::STRING, "\"Test String\"", "Test String", 9),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 9),

            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 10),

            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 11),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 11),
            compilation::token(compilation::token_type::_EOF, "", 0, 11),
        };

        const auto [tokens, errors] = get_tokens(chars);
        for (u32 i = 0; i < tokens.size(); ++i) {
            ASSERT_EQ(tokens[i], expected[i]);
        }
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, SimpleNumParse1) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::INT, "1", 1, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 1, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)};

        const auto [statements, errors] = get_statements(tokens);

        const ast::literal expected = ast::literal(1);

        const ast::expression& actual = *dynamic_cast<const ast::expression_stmt*>(statements.front().get())->m_expression;

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, SimpleNumParse2) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::INT, "1", 1, 1),
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            compilation::token(compilation::token_type::INT, "2", 2, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 2, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)};

        const auto [statements, errors] = get_statements(tokens);

        const ast::expression& actual = *dynamic_cast<const ast::expression_stmt*>(statements.front().get())->m_expression;

        const ast::add_expr expected{
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            std::make_unique<ast::literal>(1),
            std::make_unique<ast::literal>(2)};

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, SimpleNumParse3) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::INT, "1", 1, 1),
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            compilation::token(compilation::token_type::INT, "2", 2, 1),
            compilation::token(compilation::token_type::STAR, "*", 0, 1),
            compilation::token(compilation::token_type::INT, "5", 5, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 5, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)};

        const auto [statements, errors] = get_statements(tokens);

        const ast::expression& actual = *dynamic_cast<const ast::expression_stmt*>(statements.front().get())->m_expression;

        expr_uptr left = std::make_unique<ast::mul_expr>(
            compilation::token(compilation::token_type::STAR, "*", 0, 1),
            std::make_unique<ast::literal>(2),
            std::make_unique<ast::literal>(5)
        );

        const ast::add_expr expected{
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            std::make_unique<ast::literal>(1),
            std::move(left)};

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    /*TEST(COMPILER, SimpleNumParseError1) {
        std::vector<compiler::token> tokens = {
            compiler::token(compiler::token_type::INT, "1", 1, 1),
            compiler::token(compiler::token_type::PLUS, "+", 0, 1),
            compiler::token(compiler::token_type::INT, "2", 2, 1),
            compiler::token(compiler::token_type::STAR, "*", 0, 1),
            compiler::token(compiler::token_type::INT, "5", 5, 1),
            compiler::token(compiler::token_type::MINUS, "-", 0, 1),
            compiler::token(compiler::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);
        EXPECT_EQ(statements.size(), 0);
        EXPECT_EQ(errors.size(), 1);
        EXPECT_EQ(errors[0].m_message, "error: expected expression after '-'");
    }*/

    TEST(COMPILER, GroupNumParse) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::LEFT_PAREN, "(", 1, 1),
            compilation::token(compilation::token_type::INT, "1", 1, 1),
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            compilation::token(compilation::token_type::INT, "2", 2, 1),
            compilation::token(compilation::token_type::RIGHT_PAREN, ")", 1, 1),
            compilation::token(compilation::token_type::STAR, "*", 0, 1),
            compilation::token(compilation::token_type::INT, "5", 5, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 5, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)};

        const auto [statements, errors] = get_statements(tokens);

        const ast::expression& actual = *dynamic_cast<const ast::expression_stmt*>(statements.front().get())->m_expression;

        auto left = std::make_unique<ast::grouping>(std::make_unique<ast::add_expr>(
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            std::make_unique<ast::literal>(1),
            std::make_unique<ast::literal>(2)
        ));

        const ast::mul_expr expected{
            compilation::token(compilation::token_type::STAR, "*", 0, 1),
            std::move(left),
            std::make_unique<ast::literal>(5)};

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, VariableDeclaration0) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::IDENTIFIER, "u16", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "number", 0, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)};

        const auto [statements, errors] = get_statements(tokens);

        const auto& actual = *static_cast<const ast::variable_declaration*>(statements.front().get());

        const ast::variable_declaration expected{ast::make_type_from_prim(ast::primitive_kind::U16), "number"};

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, VariableDeclaration1) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::IDENTIFIER, "u16", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "number", 0, 1),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 1),
            compilation::token(compilation::token_type::INT, "2", 2, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)};

        const auto [statements, errors] = get_statements(tokens);

        const auto& actual = *static_cast<const ast::variable_declaration*>(statements.front().get());

        const ast::variable_declaration expected{ast::make_type_from_prim(ast::primitive_kind::U16), "number", 2};

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, SimpleBlock) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 1),
            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)};

        const auto [statements, errors] = get_statements(tokens);

        const ast::block& actual = *dynamic_cast<const ast::block*>(statements.front().get());

        std::list<stmnt_uptr> expected_statements{};
        expected_statements.push_back(std::move(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::identifier>(compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 1)))));

        const ast::block expected{std::move(expected_statements)};

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, SimpleBlockError) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "b", 0, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 1),
            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)};

        const auto [statements, errors] = get_statements(tokens);

        const ast::block& actual = *dynamic_cast<const ast::block*>(statements.front().get());

        std::list<stmnt_uptr> expected_statements{};
        expected_statements.push_back(std::move(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::identifier>(compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 1)))));

        const ast::block expected{std::move(expected_statements)};

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_NE(expected, actual);
    }

    TEST(COMPILER, Program1) {
        const std::string code = "i32 x = 0; i32 y = 1; while (x < y) { x = x + y; }";

        auto [tokens, lex_errors] = get_tokens(code);

        const auto [statements, parse_errors] = get_statements(tokens);

        ASSERT_EQ(statements.size(), 3) << "expected 3 statements";
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);

        std::list<stmnt_uptr> expected;

        std::list<stmnt_uptr> block_stmnt;

        block_stmnt.push_back(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::assign_expr>(
                std::make_unique<ast::identifier>("x"),
                std::make_unique<ast::add_expr>(
                    compilation::token(compilation::token_type::PLUS, "+", 0, 1),
                    std::make_unique<ast::identifier>("x"),
                    std::make_unique<ast::identifier>("y")
                ))));
        expected.push_back(std::make_unique<ast::variable_declaration>(ast::make_type_from_prim(ast::primitive_kind::I32), "x", (u16)0));
        expected.push_back(std::make_unique<ast::variable_declaration>(ast::make_type_from_prim(ast::primitive_kind::I32), "y", (u16)1));
        expected.push_back(std::make_unique<ast::while_stmt>(
            std::make_unique<ast::compare_expr>(
                compilation::token(compilation::token_type::LESS, "<", 0, 1),
                std::make_unique<ast::identifier>("x"),
                std::make_unique<ast::identifier>("y")
            ),
            std::make_unique<ast::block>(
                std::move(block_stmnt))));

        EXPECT_EQ(expected, statements);
    }

    TEST(COMPILER, OrderOfOperationsProgram) {
        const std::string code = "i32 x = 1 + 2 * 3 - 4 / 5;";

        auto [tokens, lex_errors] = get_tokens(code);

        const auto [statements, parse_errors] = get_statements(tokens);

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);

        std::list<stmnt_uptr> expected;

        expected.push_back(std::make_unique<ast::variable_declaration>(ast::make_type_from_prim(ast::primitive_kind::I32), "x",
                                                                       std::make_unique<ast::sub_expr>(
                                                                           compilation::token(compilation::token_type::MINUS, "-", 0, 1),
                                                                           std::make_unique<ast::add_expr>(
                                                                               compilation::token(compilation::token_type::PLUS, "+", 0, 1),
                                                                               std::make_unique<ast::literal>((u16)1),
                                                                               std::make_unique<ast::mul_expr>(
                                                                                   compilation::token(compilation::token_type::STAR, "*", 0, 1),
                                                                                   std::make_unique<ast::literal>((u16)2),
                                                                                   std::make_unique<ast::literal>((u16)3)
                                                                               )),
                                                                           std::make_unique<ast::div_expr>(
                                                                               compilation::token(compilation::token_type::SLASH, "/", 0, 1),
                                                                               std::make_unique<ast::literal>((u16)4),
                                                                               std::make_unique<ast::literal>((u16)5)
                                                                           ))));

        EXPECT_EQ(expected, statements);
    }

    TEST(COMPILER, Foreach1) {
        const std::string code = "u32 y = 0; foreach (u32 item : iterable) { y = y + item; }";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [statements, parse_errors] = get_statements(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(statements.size(), 2);
        std::list<stmnt_uptr> expected;
        expected.push_back(std::make_unique<ast::variable_declaration>(ast::make_type_from_prim(ast::primitive_kind::U32), "y", (u16)0));
        std::list<stmnt_uptr> block_stmnts;
        block_stmnts.push_back(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::assign_expr>(
                std::make_unique<ast::identifier>("y"),
                std::make_unique<ast::add_expr>(
                    compilation::token(compilation::token_type::PLUS, "+", 0, 1),
                    std::make_unique<ast::identifier>("y"),
                    std::make_unique<ast::identifier>("item")
                ))));
        expected.push_back(std::make_unique<ast::foreach_stmt>(
            ast::parameter{
                ast::make_type_from_prim(ast::primitive_kind::U32),
                "item",
            },
            std::make_unique<ast::identifier>("iterable"),
            std::make_unique<ast::block>(std::move(block_stmnts))
        ));
        EXPECT_EQ(expected, statements);
    }

    TEST(COMPILER, Match1) {
        const std::string code = "return match (var_0; var_1; var_2) { 0, 1 -> \"Cool\", 2 -> \"Not cool\", else -> \"Default\" };";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [statements, parse_errors] = get_statements(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(statements.size(), 1);

        std::vector<expr_uptr> vars;
        vars.push_back(std::make_unique<ast::identifier>("var_0"));
        vars.push_back(std::make_unique<ast::identifier>("var_1"));
        vars.push_back(std::make_unique<ast::identifier>("var_2"));

        std::vector<ast::match_expr::matches_t> outer;
        outer.emplace_back(std::make_unique<ast::literal>((u16)0), std::make_unique<ast::literal>("Cool"));
        outer.emplace_back(std::make_unique<ast::literal>((u16)1), std::make_unique<ast::literal>("Cool"));
        outer.emplace_back(std::make_unique<ast::literal>((u16)2), std::make_unique<ast::literal>("Not cool"));

        std::list<stmnt_uptr> expected;
        expected.push_back(std::make_unique<ast::return_stmt>(
            std::make_unique<ast::match_expr>(
                std::move(vars),
                std::move(outer),
                std::make_unique<ast::literal>("Default")
            )));

        EXPECT_EQ(expected, statements);
    }

    TEST(COMPILER, Call1) {
        const std::string code = "doSomething(1, 2 + 3);";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [statements, parse_errors] = get_statements(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(statements.size(), 1);

        std::vector<expr_uptr> args;
        args.push_back(std::make_unique<ast::literal>((u16)1));
        args.push_back(std::make_unique<ast::add_expr>(
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            std::make_unique<ast::literal>((u16)2),
            std::make_unique<ast::literal>((u16)3)
        ));

        std::list<stmnt_uptr> expected;
        expected.push_back(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::call_expr>(
                compilation::token(compilation::token_type::IDENTIFIER, "doSomething", 0, 1),
                std::make_unique<ast::identifier>("doSomething"),
                std::move(args)
            )));

        EXPECT_EQ(expected, statements);
    }

    TEST(COMPILER, FormatOperatorParsesAsDcFormatCall) {
        const std::string code = "\"launched custom gas mask %s %s script\" $ test_string, replacement2;";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [statements, parse_errors] = get_statements(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(statements.size(), 1);

        std::vector<expr_uptr> args;
        args.push_back(std::make_unique<ast::literal>("launched custom gas mask %s %s script"));
        args.push_back(std::make_unique<ast::identifier>("test_string"));
        args.push_back(std::make_unique<ast::identifier>("replacement2"));

        std::list<stmnt_uptr> expected;
        expected.push_back(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::call_expr>(
                compilation::token(compilation::token_type::DOLLAR, "$", 0, 1),
                std::make_unique<ast::sid_identifier>("#dc:format"),
                std::move(args)
            )));

        EXPECT_EQ(expected, statements);
    }

    TEST(COMPILER, FormatOperatorBindsBeforeDisplayOperator) {
        const std::string code = ">> \"launched custom gas mask script, version %d\" $ 1;";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [statements, parse_errors] = get_statements(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(statements.size(), 1);

        std::vector<expr_uptr> format_args;
        format_args.push_back(std::make_unique<ast::literal>("launched custom gas mask script, version %d"));
        format_args.push_back(std::make_unique<ast::literal>((u16)1));

        std::vector<expr_uptr> display_args;
        display_args.push_back(std::make_unique<ast::call_expr>(
            compilation::token(compilation::token_type::DOLLAR, "$", 0, 1),
            std::make_unique<ast::sid_identifier>("#dc:format"),
            std::move(format_args)
        ));

        std::list<stmnt_uptr> expected;
        expected.push_back(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::call_expr>(
                compilation::token(compilation::token_type::GREATER_GREATER, ">>", 0, 1),
                std::make_unique<ast::sid_identifier>("#display"),
                std::move(display_args)
            )));

        EXPECT_EQ(expected, statements);
    }

    TEST(COMPILER, PseudoCForCompilerEmitsCompilerOperators) {
        std::vector<expr_uptr> format_args;
        format_args.push_back(std::make_unique<ast::literal>("state %d"));
        format_args.push_back(std::make_unique<ast::literal>((u16)7));

        auto format_call = std::make_unique<ast::call_expr>(
            compilation::token(compilation::token_type::DOLLAR, "$", 0, 1),
            std::make_unique<ast::sid_identifier>("dc:format"),
            std::move(format_args)
        );

        std::vector<expr_uptr> display_args;
        display_args.push_back(std::move(format_call));
        ast::call_expr display_call(
            compilation::token(compilation::token_type::GREATER_GREATER, ">>", 0, 1),
            std::make_unique<ast::sid_identifier>("display"),
            std::move(display_args)
        );

        EXPECT_EQ(display_call.to_c_for_compiler_string(), ">> \"state %d\" $ 7");

        ast::sid_identifier sid{"custom-state"};
        EXPECT_EQ(sid.to_c_for_compiler_string(), "#custom-state");

        std::vector<expr_uptr> go_args;
        go_args.push_back(std::make_unique<ast::sid_identifier>("next-state"));
        go_args.push_back(std::make_unique<ast::literal>((u16)1));
        ast::call_expr go_call(
            compilation::token(compilation::token_type::EQUAL_GREATER, "=>", 0, 1),
            std::make_unique<ast::sid_identifier>("go"),
            std::move(go_args)
        );

        EXPECT_EQ(go_call.to_c_for_compiler_string(), "=> #next-state");
    }

    TEST(COMPILER, FormatOperatorUsesGlobalDcFormat) {
        const std::string code =
            "u32 main(string test_string) {"
            "    string message = \"launched custom gas mask %s script\" $ test_string;"
            "    return 0;"
            "}";

        auto [tokens, lex_errors] = get_tokens(code);
        ASSERT_EQ(lex_errors.size(), 0) << (lex_errors.empty() ? "" : lex_errors[0].m_message);

        auto [program, types, parse_errors] = get_parse_results(tokens);
        ASSERT_EQ(parse_errors.size(), 0) << (parse_errors.empty() ? "" : parse_errors[0].m_message);

        compilation::scope scope{types};
        compilation::add_global_functions(scope);

        const ast::full_type* format_type = scope.lookup("#dc:format");
        ASSERT_NE(format_type, nullptr);
        ASSERT_TRUE(std::holds_alternative<ast::function_type>(*format_type));
        EXPECT_TRUE(std::get<ast::function_type>(*format_type).m_isVariadic);

        const std::vector<ast::semantic_check_error> semantic_errors = program.check_semantics(scope);
        ASSERT_EQ(semantic_errors.size(), 0) << (semantic_errors.empty() ? "" : semantic_errors[0].m_message);

        const auto functions = compile_to_functions(code);
        EXPECT_TRUE(functions) << (functions ? "" : functions.error());
    }

    TEST(COMPILER, Semantics1) {
        const std::string code = "if (1) { 2 / 1; }";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [statements, parse_errors] = get_statements(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(statements.size(), 1);

        std::list<stmnt_uptr> block;
        block.push_back(
            std::make_unique<ast::expression_stmt>(
                std::make_unique<ast::div_expr>(
                    compilation::token(compilation::token_type::SLASH, "/", 0, 1),
                    std::make_unique<ast::literal>((u16)2),
                    std::make_unique<ast::literal>((u16)1)
                )));

        ast::if_stmt expected_if(
            std::make_unique<ast::literal>((u16)1),
            std::make_unique<ast::block>(
            std::move(block))
        );

        const auto& rhs = *dynamic_cast<ast::if_stmt*>(statements.front().get());
        EXPECT_EQ(expected_if, rhs);

        dconstruct::compilation::scope type_env{{}};
        const auto errors = statements.front()->check_semantics(type_env);

        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexError1) {
        const std::string chars = "@";
        const auto [tokens, errors] = get_tokens(chars);

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };

        const std::vector<compilation::lexing_error> expected_errors = {
            compilation::lexing_error(1, "invalid token '@'")};

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(expected_errors, errors);
    }

    TEST(COMPILER, ParseError1) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::INT, "1", 1, 1),
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            compilation::token(compilation::token_type::_EOF, ";", 0, 1)};

        const auto [statements, errors] = get_statements(tokens);

        ASSERT_FALSE(errors.empty());

        const std::list<stmnt_uptr> expected_statements = {};
        EXPECT_EQ(statements, expected_statements);
    }

    TEST(COMPILER, ParseErrorUsingSync) {
        const std::string code =
            "using #display bar (string, u32) -> u0;"
            "u32 main() { return 0; }";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);

        EXPECT_EQ(lex_errors.size(), 0);
        ASSERT_EQ(parse_errors.size(), 1) << "Parser should report exactly one error (in the using statement) and sync to next global";
        EXPECT_TRUE(parse_errors[0].m_message.starts_with("expected 'as'")) << "got: " << parse_errors[0].m_message;
        EXPECT_EQ(program.m_declarations.size(), 1) << "main() should parse successfully after sync";
        const auto* main_fn = dynamic_cast<const ast::function_definition*>(program.m_declarations[0].get());
        ASSERT_NE(main_fn, nullptr);
        EXPECT_EQ(std::get<std::string>(main_fn->m_name), "main");
    }

    TEST(COMPILER, ParseStateScriptBasic) {
        const std::string code =
            "statescript #parse-basic {"
            "  options {"
            "    #player"
            "  }"
            "  declarations {"
            "    u32 #counter = 5;"
            "  }"
            "  state Idle {"
            "    block start {"
            "      track Base {"
            "        lambda { counter = counter - 1; }"
            "      }"
            "    }"
            "  }"
            "}";

        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);

        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        ASSERT_EQ(program.m_declarations.size(), 1);

        const auto* parsed = dynamic_cast<const ast::state_script*>(program.m_declarations[0].get());
        ASSERT_NE(parsed, nullptr);

        ASSERT_EQ(parsed->m_options.size(), 1);
        EXPECT_EQ(parsed->m_options[0], ast::sid_identifier("#player"));

        ASSERT_EQ(parsed->m_declarations.size(), 1);
        EXPECT_EQ(parsed->m_declarations[0].m_identifier, "#counter");

        ASSERT_EQ(parsed->m_states.size(), 1);
        EXPECT_EQ(parsed->m_states[0].m_name, "Idle");
        ASSERT_EQ(parsed->m_states[0].m_blocks.size(), 1);
        EXPECT_EQ(parsed->m_states[0].m_blocks[0].block_type_to_string(), "start");
        ASSERT_EQ(parsed->m_states[0].m_blocks[0].m_tracks.size(), 1);
        EXPECT_EQ(parsed->m_states[0].m_blocks[0].m_tracks[0].m_name, "Base");
        ASSERT_EQ(parsed->m_states[0].m_blocks[0].m_tracks[0].m_lambdas.size(), 1);
        const auto* lambda = std::get_if<ast::function_definition>(&parsed->m_states[0].m_blocks[0].m_tracks[0].m_lambdas[0]);
        ASSERT_NE(lambda, nullptr);
        ASSERT_EQ(lambda->m_body.m_statements.size(), 1);
    }

    TEST(COMPILER, CompileStateScriptBasicNoErrors) {
        const std::string code =
            "statescript #compile-basic {"
            "  options {"
            "    #opt_idle"
            "  }"
            "  declarations {"
            "    i32 #counter = 5;"
            "  }"
            "  state Idle {"
            "    block start {"
            "      track Base {"
            "        lambda { i32 var = #counter - 1; }"
            "      }"
            "    }"
            "  }"
            "}";

        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);

        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);

        compilation::scope scope{types};
        const auto semantic_errors = program.check_semantics(scope);
        EXPECT_EQ(semantic_errors.size(), 0);

        compilation::global_state global{};
        const auto binary_elements = program.compile_to_file(scope, global);
        ASSERT_TRUE(binary_elements) << "compile failed: " << (binary_elements ? "" : binary_elements.error());
        const auto& [bytes, size] = *binary_elements;
        std::ofstream out("compiled.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(bytes.get()), size);
        out.flush();
    }

    TEST(COMPILER, CompileStateScriptLevShoesMudHashOptionNoErrors) {
        const std::string code =
            "using #set-table-shader-material-values-param as near (string, f32) -> u0;"
            "using #C64E7DADB562F643 as string;"
            "statescript #ss-lev-shoes-mud-hack {"
            "  options {"
            "    #lev"
            "  }"
            "  declarations {"
            "    f32 #counter = 0.0;"
            "    i32 #test = 123;"
            "  }"
            "  state #lev-shoes-mud {"
            "    block start {"
            "      track main {"
            "        lambda {"
            "          #set-table-shader-material-values-param(#C64E7DADB562F643, #counter);"
            "        }"
            "      }"
            "    }"
            "  }"
            "}";

        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);

        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);

        compilation::scope scope{types};
        const auto semantic_errors = program.check_semantics(scope);
        EXPECT_EQ(semantic_errors.size(), 0);

        compilation::global_state global{};
        const auto binary_elements = program.compile_to_file(scope, global);
        ASSERT_TRUE(binary_elements) << "compile failed: " << (binary_elements ? "" : binary_elements.error());
        const auto& [bytes, size] = *binary_elements;
        std::ofstream out("compiled.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(bytes.get()), size);
        out.flush();
    }

    TEST(COMPILER, ParseStateScriptErrorNoStates) {
        const std::string code =
            "statescript #no-states {"
            "  options { #opt_idle }"
            "  declarations { u32 #counter; }"
            "}";

        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);

        EXPECT_EQ(lex_errors.size(), 0);
        ASSERT_EQ(parse_errors.size(), 1);
        EXPECT_EQ(parse_errors[0].m_message, "expected at least one state in statescript definition");
    }

    TEST(COMPILER, ParseStateScriptErrorNoBlocks) {
        const std::string code =
            "statescript #no-blocks {"
            "  options { #opt_idle }"
            "  declarations { u32 #counter; }"
            "  state Idle { }"
            "}";

        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);

        EXPECT_EQ(lex_errors.size(), 0);
        ASSERT_EQ(parse_errors.size(), 1);
        EXPECT_EQ(parse_errors[0].m_message, "expected at least one block in state Idle definition but got none");
    }

    TEST(COMPILER, ParseStateScriptErrorNoTracks) {
        const std::string code =
            "statescript #no-tracks {"
            "  options { #opt_idle }"
            "  declarations { u32 #counter; }"
            "  state Idle {"
            "    block start { }"
            "  }"
            "}";

        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);

        EXPECT_EQ(lex_errors.size(), 0);
        ASSERT_EQ(parse_errors.size(), 1);
        EXPECT_EQ(parse_errors[0].m_message, "expected at least one track in block definition but got none");
    }

    TEST(COMPILER, ParseStateScriptErrorNoLambdas) {
        const std::string code =
            "statescript #no-lambdas {"
            "  options { #opt_idle }"
            "  declarations { u32 #counter; }"
            "  state Idle {"
            "    block start {"
            "      track Base { }"
            "    }"
            "  }"
            "}";

        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);

        EXPECT_EQ(lex_errors.size(), 0);
        ASSERT_EQ(parse_errors.size(), 1);
        EXPECT_EQ(parse_errors[0].m_message, "expected at least one lambda in track Base definition but got none");
    }

    TEST(COMPILER, ParseStructType) {
        const std::string code =
            "struct Vector3 {\n"
            "\tf32 x;\n"
            "\tf32 y;\n"
            "\tf32 z;\n"
            "}";

        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(program.m_declarations.size(), 0);
        ast::struct_type expected_type;
        expected_type.m_name = "Vector3";
        expected_type.m_members.emplace_back("x", std::make_shared<ast::full_type>(ast::make_type_from_prim(ast::primitive_kind::F32)));
        expected_type.m_members.emplace_back("y", std::make_shared<ast::full_type>(ast::make_type_from_prim(ast::primitive_kind::F32)));
        expected_type.m_members.emplace_back("z", std::make_shared<ast::full_type>(ast::make_type_from_prim(ast::primitive_kind::F32)));
        EXPECT_TRUE(types.contains("Vector3"));
        EXPECT_TRUE(std::holds_alternative<ast::struct_type>(types.at("Vector3")));
        auto type = std::get<ast::struct_type>(types.at("Vector3"));
        EXPECT_EQ(type, expected_type);
    }

    TEST(COMPILER, ParseEnumType) {
        const std::string code =
            "enum Color {\n"
            "\tRed,\n"
            "\tGreen,\n"
            "\tBlue\n"
            "}";

        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(program.m_declarations.size(), 0);
        ast::enum_type expected_type;
        expected_type.m_name = "Color";
        expected_type.m_enumerators["Red"] = 0;
        expected_type.m_enumerators["Green"] = 1;
        expected_type.m_enumerators["Blue"] = 2;
        EXPECT_TRUE(types.contains("Color"));
        EXPECT_TRUE(std::holds_alternative<ast::enum_type>(types.at("Color")));
        auto type = std::get<ast::enum_type>(types.at("Color"));
        EXPECT_EQ(type, expected_type);
    }

    TEST(COMPILER, ParseEnumAccess) {
        const std::string code = "enum Keycodes { SPACE = 20 } u64 main() { return Keycodes.SPACE; }";

        auto [tokens, lex_errors] = get_tokens(code);
        auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        ASSERT_EQ(program.m_declarations.size(), 1);

        const auto* function = dynamic_cast<const ast::function_definition*>(program.m_declarations[0].get());
        ASSERT_NE(function, nullptr);
        ASSERT_EQ(function->m_body.m_statements.size(), 1);
        const auto* ret = dynamic_cast<const ast::return_stmt*>(function->m_body.m_statements.front().get());
        ASSERT_NE(ret, nullptr);
        const auto* access = dynamic_cast<const ast::enum_access*>(ret->m_expr.get());
        ASSERT_NE(access, nullptr);
        EXPECT_EQ(access->m_token.m_lexeme, "SPACE");
        EXPECT_EQ(access->m_enumName, "Keycodes");
        EXPECT_EQ(access->m_memberName, "SPACE");
        EXPECT_EQ(access->to_pseudo_c_string(), "Keycodes.SPACE");
        EXPECT_EQ(std::get<u64>(access->m_value.m_value), 20);

        compilation::scope scope{types};
        const auto semantic_errors = program.check_semantics(scope);
        EXPECT_EQ(semantic_errors.size(), 0);

        const auto functions = compile_to_functions(code);
        EXPECT_TRUE(functions) << (functions ? "" : functions.error());
    }

    TEST(COMPILER, ParseEnumAccessUnknownMemberNamesAccess) {
        const std::string code = "enum Keycodes { SPACE = 20 } u64 main() { return Keycodes.ENTER; }";

        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        ASSERT_EQ(parse_errors.size(), 1);
        EXPECT_EQ(parse_errors[0].m_message, "unknown enum member Keycodes.ENTER");
    }

    TEST(COMPILER, FullFunc1) {
        const std::string code = "struct Vector3 { f32 x; f32 y; f32 z; } enum Opcode { MOVE, LOAD, PUSH } i32 vector3_dist(Vector3 a) { return 1; }";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(program.m_declarations.size(), 1);

        compilation::scope scope{types};
        const auto semantic_errors = program.m_declarations[0]->check_semantics(scope);

        std::vector<ast::semantic_check_error> empty{};
        EXPECT_EQ(semantic_errors, empty);
    }

    TEST(COMPILER, Declaration1) {
        const std::string code = "u32 main () { u32 x = 0; u32 y = 0; return x = y; }";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0) << lex_errors[0].m_message;
        EXPECT_EQ(parse_errors.size(), 0) << parse_errors[0].m_message;
        EXPECT_EQ(program.m_declarations.size(), 1);

        compilation::scope scope{types};
        const auto semantic_errors = program.m_declarations[0]->check_semantics(scope);

        std::vector<ast::semantic_check_error> empty{};
        EXPECT_EQ(semantic_errors, empty) << semantic_errors[0].m_message;
    }

    TEST(COMPILER, NullAssignableToAnyType) {
        const std::string code =
            "using #takes-string as near (string) -> u0;"
            "using #takes-u64 as near (u64) -> u0;"
            "u64 main() {"
            "    string text = null;"
            "    u64 value = null;"
            "    u64* pointer = null;"
            "    text = null;"
            "    value = null;"
            "    pointer = null;"
            "    #takes-string(null);"
            "    #takes-u64(null);"
            "    return null;"
            "}";

        auto [tokens, lex_errors] = get_tokens(code);
        ASSERT_EQ(lex_errors.size(), 0) << (lex_errors.empty() ? "" : lex_errors[0].m_message);

        const auto [program, types, parse_errors] = get_parse_results(tokens);
        ASSERT_EQ(parse_errors.size(), 0) << (parse_errors.empty() ? "" : parse_errors[0].m_message);

        compilation::scope scope{types};
        const std::vector<ast::semantic_check_error> semantic_errors = program.check_semantics(scope);
        ASSERT_EQ(semantic_errors.size(), 0) << (semantic_errors.empty() ? "" : semantic_errors[0].m_message);
    }

    TEST(COMPILER, NullActsAsZeroInLogicalExpressions) {
        const std::string code =
            "u64 main(u64 value) {"
            "    if (null || value && null) {"
            "        return 1;"
            "    }"
            "    while (null) {"
            "        return 2;"
            "    }"
            "    if (value != null) {"
            "        return 3;"
            "    }"
            "    return 0;"
            "}";

        auto [tokens, lex_errors] = get_tokens(code);
        ASSERT_EQ(lex_errors.size(), 0) << (lex_errors.empty() ? "" : lex_errors[0].m_message);

        const auto [program, types, parse_errors] = get_parse_results(tokens);
        ASSERT_EQ(parse_errors.size(), 0) << (parse_errors.empty() ? "" : parse_errors[0].m_message);

        compilation::scope scope{types};
        const std::vector<ast::semantic_check_error> semantic_errors = program.check_semantics(scope);
        ASSERT_EQ(semantic_errors.size(), 0) << (semantic_errors.empty() ? "" : semantic_errors[0].m_message);
    }

    TEST(COMPILER, NonCallableCallSemanticErrorHasSourceLine) {
        const std::string code =
            "u64 main() {\n"
            "    u64 not_func = 1;\n"
            "    not_func();\n"
            "    return 0;\n"
            "}";

        auto [tokens, lex_errors] = get_tokens(code);
        ASSERT_EQ(lex_errors.size(), 0) << (lex_errors.empty() ? "" : lex_errors[0].m_message);

        const auto [program, types, parse_errors] = get_parse_results(tokens);
        ASSERT_EQ(parse_errors.size(), 0) << (parse_errors.empty() ? "" : parse_errors[0].m_message);

        compilation::scope scope{types};
        const std::vector<ast::semantic_check_error> semantic_errors = program.check_semantics(scope);
        ASSERT_EQ(semantic_errors.size(), 1);
        EXPECT_EQ(semantic_errors[0].m_message, "expected callable type but got u64");
        ASSERT_NE(semantic_errors[0].m_expr, nullptr);

        const std::optional<compilation::source_location> loc = semantic_errors[0].m_expr->source_location();
        ASSERT_TRUE(loc);
        EXPECT_EQ(loc->m_line, 3);
    }

    TEST(COMPILER, Using1) {
        const std::string code =
            "using #display as far (string, u32) -> u0;"
            "using #5445173390656D6D as near (string, u32, u32) -> string;"
            "u32 main() {"
            "    string message = #5445173390656D6D(\"Hello World from DC version %d.%d\", 0, 0);"
            "    #display(message, 19);"
            "    return 0;"
            "}";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(program.m_declarations.size(), 3);

        compilation::scope scope{types};
        std::vector<ast::semantic_check_error> semantic_errors = program.check_semantics(scope);

        const ast::full_type* alias_type = scope.lookup("#5445173390656D6D");
        ASSERT_NE(alias_type, nullptr);
        ASSERT_TRUE(std::holds_alternative<ast::function_type>(*alias_type));
        EXPECT_EQ(std::get<ast::function_type>(*alias_type).m_distanceType, ast::function_type::DISTANCE::NEAR);

        std::vector<ast::semantic_check_error> empty{};
        EXPECT_EQ(semantic_errors, empty);
    }

    TEST(COMPILER, UsingVariadicFormat) {
        const std::string code =
            "using #dc:format as far(string, ...) -> string;"
            "u32 main() {"
            "    string message1 = #dc:format(\"gas mask %16X %16X\", 1, 2);"
            "    string message2 = #dc:format(\"gas mask %16X %16X %s\", 1, 2, \"test\");"
            "    return 0;"
            "}";

        auto [tokens, lex_errors] = get_tokens(code);
        ASSERT_EQ(lex_errors.size(), 0) << (lex_errors.empty() ? "" : lex_errors[0].m_message);

        auto [program, types, parse_errors] = get_parse_results(tokens);
        ASSERT_EQ(parse_errors.size(), 0) << (parse_errors.empty() ? "" : parse_errors[0].m_message);
        ASSERT_EQ(program.m_declarations.size(), 2);

        const auto* using_decl = dynamic_cast<const ast::using_declaration*>(program.m_declarations[0].get());
        ASSERT_NE(using_decl, nullptr);
        ASSERT_TRUE(std::holds_alternative<ast::function_type>(using_decl->m_type));

        const auto& format_type = std::get<ast::function_type>(using_decl->m_type);
        EXPECT_EQ(format_type.m_distanceType, ast::function_type::DISTANCE::FAR);
        EXPECT_TRUE(format_type.m_isVariadic);
        ASSERT_EQ(format_type.m_arguments.size(), 1);
        EXPECT_EQ(*format_type.m_arguments[0].second, ast::make_type_from_prim(ast::primitive_kind::STRING));
        EXPECT_EQ(*format_type.m_return, ast::make_type_from_prim(ast::primitive_kind::STRING));

        compilation::scope scope{types};
        std::vector<ast::semantic_check_error> semantic_errors = program.check_semantics(scope);
        ASSERT_EQ(semantic_errors.size(), 0) << (semantic_errors.empty() ? "" : semantic_errors[0].m_message);

        const ast::full_type* alias_type = scope.lookup("#dc:format");
        ASSERT_NE(alias_type, nullptr);
        ASSERT_TRUE(std::holds_alternative<ast::function_type>(*alias_type));
        EXPECT_TRUE(std::get<ast::function_type>(*alias_type).m_isVariadic);
    }

    TEST(COMPILER, FullCompile1) {
        const std::string code =
            "i32 main() {"
            "   u64 x = 1;"
            "   u64 y = x * 2;"
            "   return 0;"
            "}";

        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(program.m_declarations.size(), 1);

        compilation::scope scope{types};
        std::vector<ast::semantic_check_error> semantic_errors = program.check_semantics(scope);
        ASSERT_EQ(semantic_errors.size(), 0);

        const auto binary = program.compile_to_file(scope);
        ASSERT_TRUE(binary.has_value());
        const auto& [bytes, size] = *binary;
        std::ofstream out("compiled.bin", std::ios::binary);
        out.write(reinterpret_cast<const char*>(bytes.get()), size);
        out.flush();
    }

    TEST(COMPILER, CastInstructions1) {
        expect_instructions(
            "i32 main() { return (i32)1.0; }",
            {
                Instruction{Opcode::LoadStaticFloatImm, 0, 0, 0},
                Instruction{Opcode::Move, 1, 0, 0},
                Instruction{Opcode::CastInteger, 1, 0, 0},
                Instruction{Opcode::Move, 0, 1, 0},
                Instruction{Opcode::Return, 0, 0, 0_r},
            }
        );
    }

    TEST(COMPILER, Load1) {
        expect_instructions(
            "i32 main(i32* p) { return *p; }",
            {
                Instruction{Opcode::Move, 0, 49, 0},
                Instruction{Opcode::LoadI32, 1, 0, 0},
                Instruction{Opcode::Move, 0, 1, 0},
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, Store1) {
        expect_instructions(
            "u16 main(i32* p) { *p = (i32)42; return 0; }",
            {
                Instruction{Opcode::Move, 0, 49, 0},
                Instruction{Opcode::LoadU16Imm, 1, 42, 0},
                Instruction{Opcode::StoreI32, 2, 0, 1},
                Instruction{Opcode::LoadU16Imm, 2, 0, 0},
                Instruction{Opcode::Move, 0, 2, 0},
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, Load2) {
        expect_instructions(
            "u64 main(u64* p) { return *p; }",
            {
                Instruction{Opcode::Move, 0, 49, 0},
                Instruction{Opcode::LoadU64, 1, 0, 0},
                Instruction{Opcode::Move, 0, 1, 0},
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, Store2) {
        expect_instructions(
            "u16 main(u64* p) { *p = 42; return 0; }",
            {
                Instruction{Opcode::Move, 0, 49, 0},
                Instruction{Opcode::LoadU16Imm, 1, 42, 0},
                Instruction{Opcode::StoreU64, 2, 0, 1},
                Instruction{Opcode::LoadU16Imm, 2, 0, 0},
                Instruction{Opcode::Move, 0, 2, 0},
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, ReturnLiteral) {
        expect_instructions(
            "i32 main() { return 1; }",
            {
                Instruction{Opcode::LoadU16Imm, 0, 1, 0},
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, ReturnNullLoadsZero) {
        expect_instructions(
            "u64 main() { return null; }",
            {
                Instruction{Opcode::LoadU16Imm, 0, 0, 0},
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, MatchArrayCompilationLoadsFromSymbolTable) {
        const std::string code =
            "u64 main() {"
            "   string test = \"test\";"
            "   u64 a = 0;"
            "   u64 b = match (a) { 0 -> #ellie, 1 -> #dina };"
            "   return b;"
            "}";

        auto functions = compile_to_functions(code);
        ASSERT_TRUE(functions) << functions.error();
        ASSERT_EQ(functions->size(), 1);

        const compilation::function_context& fn = (*functions)[0];
        EXPECT_EQ(
            fn.m_instructions,
            (std::vector<Instruction>{
                Instruction{Opcode::LoadStaticPointerImm, 0, 0, 0},
                Instruction{Opcode::LoadU16Imm, 1, 0, 0},
                Instruction{Opcode::IAddImm, 1, 1, 1},
                Instruction{Opcode::LoadStaticInt, 2, 1, 0},
                Instruction{Opcode::Move, 0, 2, 0},
                Instruction{Opcode::Return, 0, 0, 0},
            })
        );

        ASSERT_EQ(fn.m_symbolTable.size(), 3);
        EXPECT_EQ(fn.m_symbolTable[0], 0);
        EXPECT_EQ(fn.m_symbolTable[1], SID("ellie"));
        EXPECT_EQ(fn.m_symbolTable[2], SID("dina"));

        ASSERT_EQ(fn.m_symbolTableEntryPointers.size(), 3);
        EXPECT_EQ(fn.m_symbolTableEntryPointers[0], compilation::function_context::SYMBOL_TABLE_POINTER_KIND::STRING);
        EXPECT_EQ(fn.m_symbolTableEntryPointers[1], compilation::function_context::SYMBOL_TABLE_POINTER_KIND::NONE);
        EXPECT_EQ(fn.m_symbolTableEntryPointers[2], compilation::function_context::SYMBOL_TABLE_POINTER_KIND::NONE);
    }

    TEST(COMPILER, MatchArrayCompilationElseLoadsDefault) {
        const std::string code =
            "u64 main(u64 a) {"
            "   return match (a) { 0 -> #ellie, 2 -> #dina, else -> #joel };"
            "}";

        auto functions = compile_to_functions(code);
        ASSERT_TRUE(functions) << functions.error();
        ASSERT_EQ(functions->size(), 1);

        std::vector<Instruction> expected{
            Instruction{Opcode::Move, 0, 49, 0},
            Instruction{Opcode::LoadU16Imm, 1, 2, 0},
            Instruction{Opcode::IGreaterThan, 1, 0, 1},
            Instruction{Opcode::BranchIfNot, 6, 1, 0},
            Instruction{Opcode::LoadStaticU64Imm, 1, 1, 0},
            Instruction{Opcode::Branch, 7, 0, 0},
            Instruction{Opcode::LoadStaticInt, 1, 0, 0},
            Instruction{Opcode::Move, 0, 1, 0},
            Instruction{Opcode::Return, 0, 0, 0},
        };
        const compilation::function_context& fn = (*functions)[0];
        EXPECT_EQ(fn.m_instructions, expected);

        ASSERT_EQ(fn.m_symbolTable.size(), 5);
        EXPECT_EQ(fn.m_symbolTable[0], SID("ellie"));
        EXPECT_EQ(fn.m_symbolTable[1], SID("joel"));
        EXPECT_EQ(fn.m_symbolTable[2], SID("dina"));
        EXPECT_EQ(fn.m_symbolTable[3], 0);
        EXPECT_EQ(fn.m_symbolTable[4], 2);
    }

    TEST(COMPILER, IfElseReturn) {
        expect_instructions(
            "i32 main() { if (1) { return 2; } return 3; }",
            {
                Instruction{Opcode::LoadU16Imm, 0, 1, 0},
                Instruction{Opcode::BranchIfNot, 4, 0, 0},
                Instruction{Opcode::LoadU16Imm, 0, 2, 0},
                Instruction{Opcode::Branch, 6, 0, 0},
                Instruction{Opcode::LoadU16Imm, 1, 3, 0},
                Instruction{Opcode::Move, 0, 1, 0},
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, VoidReturn) {
        expect_instructions(
            "u0 main() { return; }",
            {
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, VoidEarlyReturn) {
        expect_instructions(
            "u0 main(i32 a) { if (a) { return; } return; }",
            {
                Instruction{Opcode::Move, 0, 49, 0},
                Instruction{Opcode::BranchIfNot, 3, 0, 0},
                Instruction{Opcode::Branch, 3, 0, 0},
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, VoidReturnNotLastStatement) {
        expect_instructions(
            "u0 main() { if (1) { return; } u16 x = 0; }",
            {
                Instruction{Opcode::LoadU16Imm, 0, 1, 0},
                Instruction{Opcode::BranchIfNot, 3, 0, 0},
                Instruction{Opcode::Branch, 4, 0, 0},
                Instruction{Opcode::LoadU16Imm, 0, 0, 0},
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, WhileLoop) {
        expect_instructions(
            "i32 main() { i32 x = 0; i32 y = 1; while (x < y) { x = x + y; } return x; }",
            {
                Instruction{Opcode::LoadU16Imm, 0, 0, 0},
                Instruction{Opcode::LoadU16Imm, 1, 1, 0},
                Instruction{Opcode::ILessThan, 2, 0, 1},
                Instruction{Opcode::BranchIfNot, 6, 2, 0},
                Instruction{Opcode::IAdd, 0, 0, 1},
                Instruction{Opcode::Branch, 2, 0, 0},
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, LogicalAndShortCircuit) {
        auto functions = compile_to_functions("i32 main() { if (0 && 42) { return 1; } return 0; }");
        ASSERT_TRUE(functions) << functions.error();
        ASSERT_FALSE(functions->empty());

        const auto& instructions = (*functions)[0].m_instructions;
        ASSERT_FALSE(instructions.empty());

        const auto lhs_branch_it = std::find_if(
            instructions.begin(),
            instructions.end(),
            [](const Instruction& instruction) {
                return instruction.opcode == Opcode::BranchIfNot;
            }
        );
        ASSERT_NE(lhs_branch_it, instructions.end());

        const auto rhs_load_it = std::find_if(
            instructions.begin(),
            instructions.end(),
            [](const Instruction& instruction) {
                return instruction.opcode == Opcode::LoadU16Imm && instruction.operand1 == 42;
            }
        );
        ASSERT_NE(rhs_load_it, instructions.end());

        const u16 lhs_branch_target = lhs_branch_it->destination | (lhs_branch_it->operand2 << 8);
        const u16 rhs_load_index = static_cast<u16>(std::distance(instructions.begin(), rhs_load_it));
        const u16 lhs_branch_index = static_cast<u16>(std::distance(instructions.begin(), lhs_branch_it));

        EXPECT_LT(lhs_branch_index, rhs_load_index);
        EXPECT_GT(lhs_branch_target, rhs_load_index);
        EXPECT_EQ(instructions.back().opcode, Opcode::Return);
    }

    TEST(COMPILER, IfLogicalAndBranchesDirectly) {
        expect_instructions(
            "i32 main(i32 a, i32 b) { if (a && b) { return 1; } return 0; }",
            {
                Instruction{Opcode::Move, 0, 49, 0},
                Instruction{Opcode::Move, 1, 50, 0},
                Instruction{Opcode::BranchIfNot, 7, 0, 0},
                Instruction{Opcode::BranchIfNot, 7, 1, 0},
                Instruction{Opcode::LoadU16Imm, 2, 1, 0},
                Instruction{Opcode::Move, 0, 2, 0},
                Instruction{Opcode::Branch, 9, 0, 0},
                Instruction{Opcode::LoadU16Imm, 3, 0, 0},
                Instruction{Opcode::Move, 0, 3, 0},
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, IfLogicalOrBranchesDirectly) {
        expect_instructions(
            "i32 main(i32 a, i32 b) { if (a || b) { return 1; } return 0; }",
            {
                Instruction{Opcode::Move, 0, 49, 0},
                Instruction{Opcode::Move, 1, 50, 0},
                Instruction{Opcode::BranchIf, 4, 0, 0},
                Instruction{Opcode::BranchIfNot, 7, 1, 0},
                Instruction{Opcode::LoadU16Imm, 2, 1, 0},
                Instruction{Opcode::Move, 0, 2, 0},
                Instruction{Opcode::Branch, 9, 0, 0},
                Instruction{Opcode::LoadU16Imm, 3, 0, 0},
                Instruction{Opcode::Move, 0, 3, 0},
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, IfGroupedLogicalBranchesDirectly) {
        expect_instructions(
            "i32 main(i32 a, i32 b) { if ((a && b)) { return 1; } return 0; }",
            {
                Instruction{Opcode::Move, 0, 49, 0},
                Instruction{Opcode::Move, 1, 50, 0},
                Instruction{Opcode::BranchIfNot, 7, 0, 0},
                Instruction{Opcode::BranchIfNot, 7, 1, 0},
                Instruction{Opcode::LoadU16Imm, 2, 1, 0},
                Instruction{Opcode::Move, 0, 2, 0},
                Instruction{Opcode::Branch, 9, 0, 0},
                Instruction{Opcode::LoadU16Imm, 3, 0, 0},
                Instruction{Opcode::Move, 0, 3, 0},
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, Subscript1) {
        expect_instructions(
            "u0 main(u16* bytes) { u16 my_byte = bytes[2]; }",
            {
                Instruction{Opcode::Move, 0_r, 49_r},
                Instruction{Opcode::LoadU16Imm, 2_r, 2_r, 0_r},
                Instruction{Opcode::IMulImm, 1_r, 2_r, 2_imm},
                Instruction{Opcode::IAdd, 1_r, 0_r, 1_r},
                Instruction{Opcode::LoadU16, 1_r, 1_r},
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, Subscript2) {
        expect_instructions(
            "u0 main(f32* floats) { floats[2] = 0.69; }",
            {
                Instruction{Opcode::Move, 0_r, 49_r},
                Instruction{Opcode::LoadU16Imm, 1_r, 2_r, 0_r},
                Instruction{Opcode::IMulImm, 2_r, 1_r, 4_imm},
                Instruction{Opcode::IAdd, 2_r, 0_r, 2_r},
                Instruction{Opcode::LoadStaticFloatImm, 1_r, 0, 0},
                Instruction{Opcode::StoreFloat, 3_r, 2_r, 1_r},
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, SizeofNormalType) {
        const std::string code = "sizeof(u32);";
        auto [tokens, lex_errors] = get_tokens(code);
        ASSERT_EQ(lex_errors.size(), 0);
        auto [statements, parse_errors] = get_statements(tokens);
        ASSERT_EQ(parse_errors.size(), 0);
        ASSERT_EQ(statements.size(), 1);

        const auto* expr_stmt = dynamic_cast<const ast::expression_stmt*>(statements.front().get());
        ASSERT_NE(expr_stmt, nullptr);
        const auto* sz = dynamic_cast<const ast::sizeof_expr*>(expr_stmt->m_expression.get());
        ASSERT_NE(sz, nullptr);
        ASSERT_TRUE(std::holds_alternative<ast::full_type>(sz->m_operand));
        const auto& ty = std::get<ast::full_type>(sz->m_operand);
        ASSERT_TRUE(std::holds_alternative<ast::primitive_type>(ty));
        EXPECT_EQ(std::get<ast::primitive_type>(ty).m_type, ast::primitive_kind::U32);

        auto functions = compile_to_functions("u64 main() { return sizeof(u32); }");
        ASSERT_TRUE(functions) << functions.error();
        ASSERT_FALSE(functions->empty());
        ASSERT_EQ((*functions)[0].m_instructions.size(), 2);
        EXPECT_EQ((*functions)[0].m_instructions[0].opcode, Opcode::LoadU16Imm);
        EXPECT_EQ((*functions)[0].m_instructions[0].operand1, 4);
        EXPECT_EQ((*functions)[0].m_instructions[0].operand2, 0);
        EXPECT_EQ((*functions)[0].m_instructions[1].opcode, Opcode::Return);
    }

    TEST(COMPILER, SizeofStructType) {
        const std::string code =
            "struct Vector3 { f32 x; f32 y; f32 z; } "
            "u64 main() { return sizeof(Vector3); }";
        auto [tokens, lex_errors] = get_tokens(code);
        ASSERT_EQ(lex_errors.size(), 0);
        auto [program, types, parse_errors] = get_parse_results(tokens);
        ASSERT_EQ(parse_errors.size(), 0);
        ASSERT_EQ(program.m_declarations.size(), 1);
        ASSERT_TRUE(types.contains("Vector3"));

        compilation::scope scope{types};
        auto semantic_errors = program.check_semantics(scope);
        ASSERT_TRUE(semantic_errors.empty());

        auto elements = program.compile_binary_elements(scope);
        ASSERT_TRUE(elements) << (elements ? "" : elements.error());
        ASSERT_FALSE((*elements).empty());
    }

    TEST(COMPILER, SizeofSmallExpression) {
        const std::string code = "u32 x = 0; sizeof(x);";
        auto [tokens, lex_errors] = get_tokens(code);
        ASSERT_EQ(lex_errors.size(), 0);
        auto [statements, parse_errors] = get_statements(tokens);
        ASSERT_EQ(parse_errors.size(), 0);
        ASSERT_EQ(statements.size(), 2);

        const auto* expr_stmt = dynamic_cast<const ast::expression_stmt*>(statements.back().get());
        ASSERT_NE(expr_stmt, nullptr);
        const auto* sz = dynamic_cast<const ast::sizeof_expr*>(expr_stmt->m_expression.get());
        ASSERT_NE(sz, nullptr);
        ASSERT_TRUE(std::holds_alternative<expr_uptr>(sz->m_operand));
        const auto* inner = dynamic_cast<const ast::identifier*>(std::get<expr_uptr>(sz->m_operand).get());
        ASSERT_NE(inner, nullptr);
        EXPECT_EQ(inner->m_name.m_lexeme, "x");
    }

    TEST(COMPILER, SizeofBiggerExpression) {
        const std::string code = "u32 a = 0; u32 b = 0; sizeof(a + b);";
        auto [tokens, lex_errors] = get_tokens(code);
        ASSERT_EQ(lex_errors.size(), 0);
        auto [statements, parse_errors] = get_statements(tokens);
        ASSERT_EQ(parse_errors.size(), 0);
        ASSERT_EQ(statements.size(), 3);

        const auto* expr_stmt = dynamic_cast<const ast::expression_stmt*>(statements.back().get());
        ASSERT_NE(expr_stmt, nullptr);
        const auto* sz = dynamic_cast<const ast::sizeof_expr*>(expr_stmt->m_expression.get());
        ASSERT_NE(sz, nullptr);
        ASSERT_TRUE(std::holds_alternative<expr_uptr>(sz->m_operand));
        const auto* inner = dynamic_cast<const ast::add_expr*>(std::get<expr_uptr>(sz->m_operand).get());
        ASSERT_NE(inner, nullptr);
    }

    static std::vector<std::string> get_semantic_error_messages(const std::string& code) {
        auto [tokens, lex_errors] = get_tokens(code);
        if (!lex_errors.empty()) {
            return {"lex error: " + lex_errors.front().m_message};
        }
        auto [program, types, parse_errors] = get_parse_results(tokens);
        if (!parse_errors.empty()) {
            return {"parse error: " + parse_errors.front().m_message};
        }
        compilation::scope scope{types};
        compilation::add_global_functions(scope);
        std::vector<std::string> messages;
        for (const auto& error : program.check_semantics(scope)) {
            messages.push_back(error.m_message);
        }
        return messages;
    }

    static bool any_message_contains(const std::vector<std::string>& messages, const std::string& needle) {
        return std::any_of(messages.begin(), messages.end(), [&needle](const std::string& message) {
            return message.find(needle) != std::string::npos;
        });
    }

    static void expect_opcodes_present(const std::string& code, const std::vector<Opcode>& opcodes) {
        auto functions = compile_to_functions(code);
        ASSERT_TRUE(functions) << functions.error();
        ASSERT_FALSE(functions->empty());
        const std::vector<Instruction>& instructions = (*functions)[0].m_instructions;
        ASSERT_FALSE(instructions.empty());
        for (const Opcode opcode : opcodes) {
            EXPECT_TRUE(std::any_of(instructions.begin(), instructions.end(), [opcode](const Instruction& instruction) {
                return instruction.opcode == opcode;
            })) << "missing opcode " << static_cast<u32>(opcode) << " in: " << code;
        }
        EXPECT_EQ(instructions.back().opcode, Opcode::Return);
    }

    static bool symbol_table_contains(const compilation::function_context& fn, const u64 value) {
        return std::find(fn.m_symbolTable.begin(), fn.m_symbolTable.end(), value) != fn.m_symbolTable.end();
    }

    [[nodiscard]] static u16 instruction_static_index(const Instruction& instruction) {
        return static_cast<u16>(instruction.operand1) | (static_cast<u16>(instruction.operand2) << 8);
    }

    [[nodiscard]] static std::optional<u64> static_argument_at_call(
        const compilation::function_context& fn,
        const sid64 callee,
        const u8 argument_index
    ) {
        std::array<std::optional<u64>, MAX_REGISTER> register_values{};

        for (const Instruction& instruction : fn.m_instructions) {
            switch (instruction.opcode) {
                case Opcode::LoadStaticU64Imm:
                case Opcode::LookupPointer: {
                    const u16 symbol_index = instruction_static_index(instruction);
                    if (symbol_index < fn.m_symbolTable.size()) {
                        register_values[instruction.destination] = fn.m_symbolTable[symbol_index];
                    } else {
                        register_values[instruction.destination] = std::nullopt;
                    }
                    break;
                }
                case Opcode::Move:
                    register_values[instruction.destination] = register_values[instruction.operand1];
                    break;
                case Opcode::Call:
                case Opcode::CallFf:
                    if (register_values[instruction.operand1] == callee) {
                        return register_values[ARGUMENT_REGISTERS_IDX + argument_index];
                    }
                    register_values[instruction.destination] = std::nullopt;
                    break;
                default:
                    if (!instruction.destination_is_immediate()) {
                        register_values[instruction.destination] = std::nullopt;
                    }
                    break;
            }
        }

        return std::nullopt;
    }

    TEST(COMPILER, LexerKeywords2) {
        const auto [tokens, errors] = get_tokens("for break continue sizeof null true false using as near far auto");

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::FOR, "for", 0, 1),
            compilation::token(compilation::token_type::BREAK, "break", 0, 1),
            compilation::token(compilation::token_type::CONTINUE, "continue", 0, 1),
            compilation::token(compilation::token_type::SIZEOF, "sizeof", 0, 1),
            compilation::token(compilation::token_type::_NULL, "null", 0, 1),
            compilation::token(compilation::token_type::TRUE, "true", 0, 1),
            compilation::token(compilation::token_type::FALSE, "false", 0, 1),
            compilation::token(compilation::token_type::USING, "using", 0, 1),
            compilation::token(compilation::token_type::AS, "as", 0, 1),
            compilation::token(compilation::token_type::NEAR, "near", 0, 1),
            compilation::token(compilation::token_type::FAR, "far", 0, 1),
            compilation::token(compilation::token_type::AUTO, "auto", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerOperatorTokens) {
        const auto [tokens, errors] = get_tokens("-> => ... ^^ >> && || ++ -- *= != $");

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::ARROW, "->", 0, 1),
            compilation::token(compilation::token_type::EQUAL_GREATER, "=>", 0, 1),
            compilation::token(compilation::token_type::DOT_DOT_DOT, "...", 0, 1),
            compilation::token(compilation::token_type::CARET_CARET, "^^", 0, 1),
            compilation::token(compilation::token_type::GREATER_GREATER, ">>", 0, 1),
            compilation::token(compilation::token_type::AMPERSAND_AMPERSAND, "&&", 0, 1),
            compilation::token(compilation::token_type::PIPE_PIPE, "||", 0, 1),
            compilation::token(compilation::token_type::PLUS_PLUS, "++", 0, 1),
            compilation::token(compilation::token_type::MINUS_MINUS, "--", 0, 1),
            compilation::token(compilation::token_type::STAR_EQUAL, "*=", 0, 1),
            compilation::token(compilation::token_type::BANG_EQUAL, "!=", 0, 1),
            compilation::token(compilation::token_type::DOLLAR, "$", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, LexerNumericSidLiteral) {
        const auto [tokens, errors] = get_tokens("#5445173390656D6D");

        ASSERT_EQ(errors.size(), 0);
        ASSERT_EQ(tokens.size(), 2);
        EXPECT_EQ(tokens[0].m_type, compilation::token_type::SID);
        EXPECT_EQ(std::get<u64>(tokens[0].m_literal), 0x5445173390656D6D);
    }

    TEST(COMPILER, LexerIntWidths) {
        const auto [tokens, errors] = get_tokens("65535 65536 4294967295 4294967296");

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::INT, "65535", static_cast<u16>(65535), 1),
            compilation::token(compilation::token_type::INT, "65536", static_cast<u32>(65536), 1),
            compilation::token(compilation::token_type::INT, "4294967295", static_cast<u32>(4294967295), 1),
            compilation::token(compilation::token_type::INT, "4294967296", static_cast<u64>(4294967296), 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1),
        };

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(errors.size(), 0);
    }

    TEST(COMPILER, ParseForLoop) {
        const std::string code = "u64 x = 0; for (u64 i = 0; i < 3; i = i + 1) { x = x + i; }";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [statements, parse_errors] = get_statements(tokens);

        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        ASSERT_EQ(statements.size(), 2);

        const auto* for_loop = dynamic_cast<const ast::for_stmt*>(statements.back().get());
        ASSERT_NE(for_loop, nullptr);
        EXPECT_NE(dynamic_cast<const ast::variable_declaration*>(for_loop->m_init.get()), nullptr);
        EXPECT_NE(dynamic_cast<const ast::compare_expr*>(for_loop->m_condition.get()), nullptr);
        EXPECT_NE(dynamic_cast<const ast::assign_expr*>(for_loop->m_incr.get()), nullptr);
        EXPECT_NE(dynamic_cast<const ast::block*>(for_loop->m_body.get()), nullptr);
    }

    TEST(COMPILER, ParseUnaryExpressions) {
        const std::string code = "u64 a = 0; -a; !a; ++a; *a;";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [statements, parse_errors] = get_statements(tokens);

        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        ASSERT_EQ(statements.size(), 5);

        auto it = statements.begin();
        ++it;
        const auto* negate = dynamic_cast<const ast::expression_stmt*>(it->get());
        ASSERT_NE(negate, nullptr);
        EXPECT_NE(dynamic_cast<const ast::negate_expr*>(negate->m_expression.get()), nullptr);

        ++it;
        const auto* logical_not = dynamic_cast<const ast::expression_stmt*>(it->get());
        ASSERT_NE(logical_not, nullptr);
        EXPECT_NE(dynamic_cast<const ast::logical_not_expr*>(logical_not->m_expression.get()), nullptr);

        ++it;
        const auto* increment = dynamic_cast<const ast::expression_stmt*>(it->get());
        ASSERT_NE(increment, nullptr);
        EXPECT_NE(dynamic_cast<const ast::post_arithmetic_expression*>(increment->m_expression.get()), nullptr);

        ++it;
        const auto* dereference = dynamic_cast<const ast::expression_stmt*>(it->get());
        ASSERT_NE(dereference, nullptr);
        EXPECT_NE(dynamic_cast<const ast::dereference_expr*>(dereference->m_expression.get()), nullptr);
    }

    TEST(COMPILER, ParseBitwiseNotFromTokens) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::TILDE, "~", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)};

        const auto [statements, errors] = get_statements(tokens);

        EXPECT_EQ(errors.size(), 0);
        ASSERT_EQ(statements.size(), 1);
        const auto* stmt = dynamic_cast<const ast::expression_stmt*>(statements.front().get());
        ASSERT_NE(stmt, nullptr);
        EXPECT_NE(dynamic_cast<const ast::bitwise_not_expr*>(stmt->m_expression.get()), nullptr);
    }

    TEST(COMPILER, ParseSubscriptAndStructAccess) {
        const std::string code = "a[1]; a.b;";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [statements, parse_errors] = get_statements(tokens);

        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        ASSERT_EQ(statements.size(), 2);

        const auto* subscript_stmt = dynamic_cast<const ast::expression_stmt*>(statements.front().get());
        ASSERT_NE(subscript_stmt, nullptr);
        EXPECT_NE(dynamic_cast<const ast::subscript_expr*>(subscript_stmt->m_expression.get()), nullptr);

        const auto* access_stmt = dynamic_cast<const ast::expression_stmt*>(statements.back().get());
        ASSERT_NE(access_stmt, nullptr);
        const auto* access = dynamic_cast<const ast::struct_access*>(access_stmt->m_expression.get());
        ASSERT_NE(access, nullptr);
        EXPECT_EQ(access->m_memberName.m_lexeme, "b");
    }

    TEST(COMPILER, ParseCastExpression) {
        const std::string code = "u64 x = (i32)1.0;";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [statements, parse_errors] = get_statements(tokens);

        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        ASSERT_EQ(statements.size(), 1);

        const auto* declaration = dynamic_cast<const ast::variable_declaration*>(statements.front().get());
        ASSERT_NE(declaration, nullptr);
        const auto* cast = dynamic_cast<const ast::cast_expr*>(declaration->m_init.get());
        ASSERT_NE(cast, nullptr);
        EXPECT_EQ(cast->m_castType, ast::make_type_from_prim(ast::primitive_kind::I32));
        EXPECT_NE(dynamic_cast<const ast::literal*>(cast->m_rhs.get()), nullptr);
    }

    TEST(COMPILER, SemanticErrorUndefinedIdentifier) {
        const auto messages = get_semantic_error_messages("u64 main() { u64 x = undefined_var; return 0; }");
        ASSERT_FALSE(messages.empty());
        EXPECT_TRUE(any_message_contains(messages, "undefined_var")) << messages.front();
    }

    TEST(COMPILER, SemanticErrorMissingReturnValue) {
        const auto messages = get_semantic_error_messages("i32 main() { i32 x = 0; }");
        ASSERT_FALSE(messages.empty());
        EXPECT_TRUE(any_message_contains(messages, "expects a value to be returned")) << messages.front();
    }

    TEST(COMPILER, SemanticErrorAddNonAddable) {
        const auto messages = get_semantic_error_messages("u64 main(string s) { u64 x = s + 1; return 0; }");
        ASSERT_FALSE(messages.empty());
        EXPECT_TRUE(any_message_contains(messages, "addable")) << messages.front();
    }

    TEST(COMPILER, SemanticErrorDereferenceNonPointer) {
        const auto messages = get_semantic_error_messages("u64 main(u64 a) { return *a; }");
        EXPECT_FALSE(messages.empty());
    }

    TEST(COMPILER, SemanticNestedScopeReadsOuterVariable) {
        const auto messages = get_semantic_error_messages("u64 main() { u64 outer = 1; { u64 inner = outer; } return outer; }");
        EXPECT_TRUE(messages.empty()) << messages.front();
    }

    TEST(COMPILER, SemanticIfElseBothReturn) {
        const auto messages = get_semantic_error_messages("i32 main(i32 a) { if (a) { return 1; } else { return 2; } }");
        EXPECT_TRUE(messages.empty()) << messages.front();
    }

    TEST(COMPILER, AddEmission) {
        expect_instructions(
            "u64 main(u64 a, u64 b) { return a + b; }",
            {
                Instruction{Opcode::Move, 0, 49, 0},
                Instruction{Opcode::Move, 1, 50, 0},
                Instruction{Opcode::IAdd, 2, 0, 1},
                Instruction{Opcode::Move, 0, 2, 0},
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, SubEmission) {
        expect_instructions(
            "u64 main(u64 a, u64 b) { return a - b; }",
            {
                Instruction{Opcode::Move, 0, 49, 0},
                Instruction{Opcode::Move, 1, 50, 0},
                Instruction{Opcode::ISub, 2, 0, 1},
                Instruction{Opcode::Move, 0, 2, 0},
                Instruction{Opcode::Return, 0, 0, 0},
            }
        );
    }

    TEST(COMPILER, MulDivEmission) {
        expect_opcodes_present("u64 main(u64 a, u64 b) { return a * b; }", {Opcode::IMul});
        expect_opcodes_present("u64 main(u64 a, u64 b) { return a / b; }", {Opcode::IDiv});
    }

    TEST(COMPILER, FloatArithmeticEmission) {
        expect_opcodes_present("f32 main(f32 a, f32 b) { return a + b; }", {Opcode::FAdd});
        expect_opcodes_present("f32 main(f32 a, f32 b) { return a - b; }", {Opcode::FSub});
        expect_opcodes_present("f32 main(f32 a, f32 b) { return a * b; }", {Opcode::FMul});
        expect_opcodes_present("f32 main(f32 a, f32 b) { return a / b; }", {Opcode::FDiv});
    }

    TEST(COMPILER, CompareEmission) {
        expect_opcodes_present("u64 main(u64 a, u64 b) { if (a < b) { return 1; } return 0; }", {Opcode::ILessThan});
        expect_opcodes_present("u64 main(u64 a, u64 b) { if (a >= b) { return 1; } return 0; }", {Opcode::IGreaterThanEqual});
        expect_opcodes_present("u64 main(u64 a, u64 b) { if (a == b) { return 1; } return 0; }", {Opcode::IEqual});
        expect_opcodes_present("u64 main(u64 a, u64 b) { if (a != b) { return 1; } return 0; }", {Opcode::INotEqual});
        expect_opcodes_present("u64 main(f32 a, f32 b) { if (a < b) { return 1; } return 0; }", {Opcode::FLessThan});
    }

    TEST(COMPILER, NegateEmission) {
        expect_opcodes_present("u64 main(i64 a) { return -a; }", {Opcode::INeg});
    }

    TEST(COMPILER, LogicalNotEmission) {
        expect_opcodes_present("u64 main(u64 a) { if (!a) { return 1; } return 0; }", {Opcode::OpLogNot});
    }

    TEST(COMPILER, PostArithmeticEmission) {
        expect_opcodes_present("u64 main(u64 a) { ++a; return a; }", {Opcode::IAddImm});
        expect_opcodes_present("u64 main(u64 a) { --a; return a; }", {Opcode::LoadStaticI64Imm, Opcode::IAdd});
    }

    TEST(COMPILER, CallNearEmission) {
        const std::string code =
            "using #foo as near (u64) -> u64;"
            "u64 main(u64 a) { return #foo(a); }";
        auto functions = compile_to_functions(code);
        ASSERT_TRUE(functions) << functions.error();
        ASSERT_FALSE(functions->empty());

        const compilation::function_context& fn = functions->back();
        EXPECT_TRUE(std::any_of(fn.m_instructions.begin(), fn.m_instructions.end(), [](const Instruction& instruction) {
            return instruction.opcode == Opcode::Call;
        }));
        EXPECT_TRUE(symbol_table_contains(fn, SID("foo")));
    }

    TEST(COMPILER, CallFarEmission) {
        const std::string code =
            "using #foo as far (u64) -> u64;"
            "u64 main(u64 a) { return #foo(a); }";
        auto functions = compile_to_functions(code);
        ASSERT_TRUE(functions) << functions.error();
        ASSERT_FALSE(functions->empty());

        const compilation::function_context& fn = functions->back();
        EXPECT_TRUE(std::any_of(fn.m_instructions.begin(), fn.m_instructions.end(), [](const Instruction& instruction) {
            return instruction.opcode == Opcode::CallFf;
        }));
        EXPECT_TRUE(symbol_table_contains(fn, SID("foo")));
    }

    TEST(COMPILER, NestedCallDoesNotClobberEarlierNearArgument) {
        const std::string code =
            "using #outer as near (u64, u64) -> u64;"
            "using #inner as near (u64) -> u64;"
            "using #first as u64;"
            "using #second as u64;"
            "u64 main() { return #outer(#first, #inner(#second)); }";

        auto functions = compile_to_functions(code);
        ASSERT_TRUE(functions) << functions.error();
        ASSERT_FALSE(functions->empty());

        const compilation::function_context& fn = functions->back();
        const std::optional<u64> first_arg = static_argument_at_call(fn, SID("outer"), 0);
        ASSERT_TRUE(first_arg.has_value());
        EXPECT_EQ(*first_arg, SID("first"));
    }

    TEST(COMPILER, NestedCallDoesNotClobberEarlierFarArguments) {
        const std::string code =
            "using #outer as far (u64, u64, u64) -> u64;"
            "using #inner as near (u64, u64) -> u64;"
            "using #first as u64;"
            "using #second as u64;"
            "using #third as u64;"
            "using #fourth as u64;"
            "u64 main() { return #outer(#first, #second, #inner(#third, #fourth)); }";

        auto functions = compile_to_functions(code);
        ASSERT_TRUE(functions) << functions.error();
        ASSERT_FALSE(functions->empty());

        const compilation::function_context& fn = functions->back();
        const std::optional<u64> first_arg = static_argument_at_call(fn, SID("outer"), 0);
        const std::optional<u64> second_arg = static_argument_at_call(fn, SID("outer"), 1);
        ASSERT_TRUE(first_arg.has_value());
        ASSERT_TRUE(second_arg.has_value());
        EXPECT_EQ(*first_arg, SID("first"));
        EXPECT_EQ(*second_arg, SID("second"));
    }

    TEST(COMPILER, SidLiteralEmission) {
        auto functions = compile_to_functions("u64 main() { u64 s = #ellie; return s; }");
        ASSERT_TRUE(functions) << functions.error();
        ASSERT_FALSE(functions->empty());

        const compilation::function_context& fn = (*functions)[0];
        EXPECT_TRUE(std::any_of(fn.m_instructions.begin(), fn.m_instructions.end(), [](const Instruction& instruction) {
            return instruction.opcode == Opcode::LoadStaticU64Imm;
        }));
        EXPECT_TRUE(symbol_table_contains(fn, SID("ellie")));
    }

    TEST(COMPILER, FloatLiteralEmission) {
        auto functions = compile_to_functions("f32 main() { return 1.5; }");
        ASSERT_TRUE(functions) << functions.error();
        ASSERT_FALSE(functions->empty());

        const compilation::function_context& fn = (*functions)[0];
        EXPECT_TRUE(std::any_of(fn.m_instructions.begin(), fn.m_instructions.end(), [](const Instruction& instruction) {
            return instruction.opcode == Opcode::LoadStaticFloatImm;
        }));
        EXPECT_TRUE(symbol_table_contains(fn, static_cast<u64>(std::bit_cast<u32>(1.5f))));
    }

    TEST(COMPILER, StringLiteralEmission) {
        auto functions = compile_to_functions("u0 main() { string s = \"hello\"; }");
        ASSERT_TRUE(functions) << functions.error();
        ASSERT_FALSE(functions->empty());

        const compilation::function_context& fn = (*functions)[0];
        EXPECT_TRUE(std::any_of(fn.m_instructions.begin(), fn.m_instructions.end(), [](const Instruction& instruction) {
            return instruction.opcode == Opcode::LoadStaticPointerImm;
        }));
        ASSERT_FALSE(fn.m_symbolTableEntryPointers.empty());
        EXPECT_EQ(fn.m_symbolTableEntryPointers[0], compilation::function_context::SYMBOL_TABLE_POINTER_KIND::STRING);
    }

    TEST(COMPILER, ForLoopEmission) {
        expect_opcodes_present(
            "u64 main() { u64 x = 0; for (u64 i = 0; i < 3; i = i + 1) { x = x + i; } return x; }",
            {Opcode::ILessThan, Opcode::BranchIfNot, Opcode::Branch, Opcode::IAdd}
        );
    }

    template <typename T>
    static void expect_manual_binary_emission(const compilation::token_type op_type, const std::string& op_lexeme, const Opcode expected_opcode) {
        T expr{
            compilation::token(op_type, op_lexeme, 0, 1),
            std::make_unique<ast::literal>((u16)6),
            std::make_unique<ast::literal>((u16)3)};

        compilation::scope scope{{}};
        const auto type_res = expr.get_type_checked(scope);
        ASSERT_TRUE(type_res) << type_res.error().m_message;

        compilation::function_context fn{};
        compilation::global_state global{};
        const resstr<reg_idx> result = expr.emit_dc(fn, global, std::nullopt);
        ASSERT_TRUE(result) << result.error();

        const std::vector<Instruction> expected = {
            Instruction{Opcode::LoadU16Imm, 0, 6, 0},
            Instruction{Opcode::LoadU16Imm, 1, 3, 0},
            Instruction{expected_opcode, 2, 0, 1},
        };
        EXPECT_EQ(fn.m_instructions, expected);
    }

    TEST(COMPILER, BitwiseAndEmission) {
        expect_manual_binary_emission<ast::bitwise_and_expr>(compilation::token_type::AMPERSAND, "&", Opcode::OpBitAnd);
    }

    TEST(COMPILER, BitwiseOrEmission) {
        expect_manual_binary_emission<ast::bitwise_or_expr>(compilation::token_type::PIPE, "|", Opcode::OpBitOr);
    }

    TEST(COMPILER, BitwiseXorEmission) {
        expect_manual_binary_emission<ast::bitwise_xor_expr>(compilation::token_type::CARET, "^", Opcode::OpBitXor);
    }

    TEST(COMPILER, ModEmission) {
        expect_manual_binary_emission<ast::mod_expr>(compilation::token_type::PERCENT, "%", Opcode::IMod);
    }

    TEST(COMPILER, TypeSizes) {
        EXPECT_EQ(ast::get_size(ast::make_type_from_prim(ast::primitive_kind::U8)), 1);
        EXPECT_EQ(ast::get_size(ast::make_type_from_prim(ast::primitive_kind::U16)), 2);
        EXPECT_EQ(ast::get_size(ast::make_type_from_prim(ast::primitive_kind::U32)), 4);
        EXPECT_EQ(ast::get_size(ast::make_type_from_prim(ast::primitive_kind::U64)), 8);
        EXPECT_EQ(ast::get_size(ast::make_type_from_prim(ast::primitive_kind::F32)), 4);
        EXPECT_EQ(ast::get_size(ast::make_type_from_prim(ast::primitive_kind::F64)), 8);
        EXPECT_EQ(ast::get_size(ast::full_type{ast::ptr_type{ast::primitive_kind::U8}}), 8);
    }

    TEST(COMPILER, TypeSignedness) {
        EXPECT_TRUE(ast::is_signed(ast::primitive_kind::I32));
        EXPECT_FALSE(ast::is_signed(ast::primitive_kind::U32));
        EXPECT_TRUE(ast::is_unsigned(ast::primitive_kind::U64));
        EXPECT_TRUE(ast::is_integral(ast::primitive_kind::U8));
        EXPECT_FALSE(ast::is_integral(ast::primitive_kind::F32));
        EXPECT_TRUE(ast::is_floating_point(ast::primitive_kind::F64));
        EXPECT_TRUE(ast::is_arithmetic(ast::primitive_kind::I16));
    }

    TEST(COMPILER, NotAssignableReason) {
        const auto same = ast::not_assignable_reason(
            ast::make_type_from_prim(ast::primitive_kind::U64),
            ast::make_type_from_prim(ast::primitive_kind::U64)
        );
        EXPECT_FALSE(same.has_value());

        const auto mismatched = ast::not_assignable_reason(
            ast::make_type_from_prim(ast::primitive_kind::U32),
            ast::make_type_from_prim(ast::primitive_kind::STRING)
        );
        EXPECT_TRUE(mismatched.has_value());
    }

    [[nodiscard]] static bool symbol_table_contains_string(const function_disassembly& fn, const std::string_view expected) {
        const SymbolTable& table = fn.m_stackFrame.m_symbolTable;
        for (u32 i = 0; i < table.m_types.size(); ++i) {
            const auto* prim = std::get_if<ast::primitive_type>(&table.m_types[i]);
            if (prim == nullptr || prim->m_type != ast::primitive_kind::STRING) {
                continue;
            }
            const char* str = table.m_location.get<const char*>(i * sizeof(u64));
            if (str != nullptr && expected == str) {
                return true;
            }
        }
        return false;
    }

    TEST(COMPILER, RetargetStateScriptLambda) {
        static SIDBase full_sidbase = *SIDBase::from_binary(TEST_DIR + "sidbase_new_fixed.bin");
        const std::filesystem::path target = TEST_ROOT / "fixtures" / "dc" / "ss-hub-lockbox.bin";

        resstr<BinaryFile> original_file = BinaryFile::from_path(target);
        ASSERT_TRUE(original_file.has_value()) << original_file.error();
        Disassembler original_disassembler(&*original_file, &full_sidbase);
        (void)original_disassembler.disassemble();
        const u64 expected_function_count = original_disassembler.get_named_functions().size();

        const std::string source =
            "u0 #wait-for-spawn@start@main@0() {\n"
            "    >> \"hello world\";\n"
            "}\n";

        compilation::global_state global{};
        const std::optional<std::vector<compilation::program_binary_element>> elements = compilation::run_compilation(source, global);
        ASSERT_TRUE(elements.has_value());
        ASSERT_EQ(elements->size(), 1);

        const auto binary = compilation::disassemble_target(target, full_sidbase, *elements, global);
        ASSERT_TRUE(binary.has_value()) << binary.error();
        const auto& [bytes, size] = *binary;
        ASSERT_GT(size, 0);

        const std::filesystem::path output = std::filesystem::temp_directory_path() / "dconstruct-retarget-ss-hub-lockbox.bin";
        {
            std::ofstream of(output, std::ios::binary | std::ios::trunc);
            ASSERT_TRUE(of.is_open());
            of.write(reinterpret_cast<const char*>(bytes.get()), size);
        }

        resstr<BinaryFile> file = BinaryFile::from_path(output);
        ASSERT_TRUE(file.has_value()) << file.error();

        Disassembler disassembler(&*file, &full_sidbase);
        (void)disassembler.disassemble();
        ASSERT_TRUE(disassembler.has_state_script());

        const ast::state_script* state_script = disassembler.get_state_script();
        EXPECT_EQ(state_script->m_declarations.size(), 31);
        ASSERT_FALSE(state_script->m_states.empty());
        EXPECT_EQ(state_script->m_states[state_script->m_initialStateIdx].m_name, "--all--");

        const std::vector<const function_disassembly*> funcs = disassembler.get_named_functions();
        EXPECT_GE(funcs.size(), expected_function_count);

        const function_disassembly* replaced = nullptr;
        const function_disassembly* untouched = nullptr;
        for (const function_disassembly* func : funcs) {
            if (func->get_id() == "wait-for-spawn@start@main@0") {
                replaced = func;
            } else if (func->get_id() == "spawn-lockbox@start@main@0") {
                untouched = func;
            }
        }

        ASSERT_NE(replaced, nullptr);
        EXPECT_EQ(replaced->m_lines.size(), 4);
        EXPECT_TRUE(symbol_table_contains_string(*replaced, "hello world"));

        ASSERT_NE(untouched, nullptr);
        EXPECT_GT(untouched->m_lines.size(), 4);
        EXPECT_FALSE(symbol_table_contains_string(*untouched, "hello world"));

        std::filesystem::remove(output);
    }

    struct entry_record {
        sid64 m_name;
        sid64 m_type;
        u64 m_offset;
    };

    [[nodiscard]] static std::vector<entry_record> read_entry_records(const BinaryFile& file) {
        std::vector<entry_record> records;
        const Entry* entries = file.m_dcheader->m_pStartOfData;
        const p64 image_base = reinterpret_cast<p64>(file.m_bytes.get());
        for (u32 i = 0; i < file.m_dcheader->m_numEntries; ++i) {
            records.push_back({entries[i].m_nameID, entries[i].m_typeId, reinterpret_cast<p64>(entries[i].m_entryPtr) - image_base});
        }
        return records;
    }

    static void write_binary_file(const std::filesystem::path& path, const std::byte* bytes, const u64 size) {
        std::ofstream of(path, std::ios::binary | std::ios::trunc);
        ASSERT_TRUE(of.is_open());
        of.write(reinterpret_cast<const char*>(bytes), size);
    }

    TEST(COMPILER, TargetPatchPreservesAllEntries) {
        static SIDBase full_sidbase = *SIDBase::from_binary(TEST_DIR + "sidbase_new_fixed.bin");
        const std::filesystem::path target = TEST_ROOT / "fixtures" / "dc" / "ss-hub-lockbox.bin";

        resstr<BinaryFile> original = BinaryFile::from_path(target);
        ASSERT_TRUE(original.has_value()) << original.error();
        const std::vector<entry_record> original_entries = read_entry_records(*original);
        ASSERT_EQ(original_entries.size(), 7);
        const u64 old_strings_start = original->m_dcheader->m_stringsOffset;
        const u64 old_text_size = original->m_dcheader->m_textSize;
        const u64 entry_table_offset = reinterpret_cast<p64>(original->m_dcheader->m_pStartOfData) - reinterpret_cast<p64>(original->m_bytes.get());
        const BinaryFile::byte_uptr original_unmapped = original->get_unmapped();

        Disassembler original_disassembler(&*original, &full_sidbase);
        (void)original_disassembler.disassemble();
        const std::vector<const function_disassembly*> original_funcs = original_disassembler.get_named_functions();
        const u64 expected_function_count = original_funcs.size();
        std::unordered_set<std::string> original_function_ids;
        for (const function_disassembly* func : original_funcs) {
            original_function_ids.insert(func->get_id());
        }

        const std::string source =
            "u0 #wait-for-spawn@start@main@0() {\n"
            "    >> \"hello world\";\n"
            "}\n";

        compilation::global_state global{};
        const std::optional<std::vector<compilation::program_binary_element>> elements = compilation::run_compilation(source, global);
        ASSERT_TRUE(elements.has_value());

        const auto binary = compilation::disassemble_target(target, full_sidbase, *elements, global);
        ASSERT_TRUE(binary.has_value()) << binary.error();
        const auto& [bytes, size] = *binary;
        ASSERT_GT(size, original->m_size);

        const std::filesystem::path output = std::filesystem::temp_directory_path() / "dconstruct-patch-ss-hub-lockbox.bin";
        write_binary_file(output, bytes.get(), size);

        resstr<BinaryFile> patched = BinaryFile::from_path(output);
        ASSERT_TRUE(patched.has_value()) << patched.error();

        const std::vector<entry_record> patched_entries = read_entry_records(*patched);
        ASSERT_EQ(patched_entries.size(), original_entries.size());

        const sid64 state_script_name = SID("ss-hub-lockbox");
        u64 state_script_entry_pointer_slot = 0;
        u64 old_state_script_offset = 0;
        for (u64 i = 0; i < original_entries.size(); ++i) {
            EXPECT_EQ(patched_entries[i].m_name, original_entries[i].m_name);
            EXPECT_EQ(patched_entries[i].m_type, original_entries[i].m_type);
            if (original_entries[i].m_name == state_script_name) {
                EXPECT_GE(patched_entries[i].m_offset, old_strings_start);
                state_script_entry_pointer_slot = entry_table_offset + i * sizeof(Entry) + offsetof(Entry, m_entryPtr);
                old_state_script_offset = original_entries[i].m_offset;
            } else {
                EXPECT_EQ(patched_entries[i].m_offset, original_entries[i].m_offset);
            }
        }
        ASSERT_NE(state_script_entry_pointer_slot, 0);
        EXPECT_TRUE(patched->gets_pointed_at(location(patched->m_bytes.get() + old_state_script_offset)));

        const u64 new_strings_start = patched->m_dcheader->m_stringsOffset;
        const u64 string_delta = new_strings_start - old_strings_start;
        ASSERT_GT(string_delta, 0);

        const BinaryFile::byte_uptr patched_unmapped = patched->get_unmapped();
        const std::byte* old_raw = original_unmapped.get();
        const std::byte* new_raw = patched_unmapped.get();
        const u8* old_reloc = reinterpret_cast<const u8*>(old_raw + old_text_size + sizeof(u32));

        for (u64 slot = 0; slot < old_strings_start / sizeof(u64); ++slot) {
            const u64 offset = slot * sizeof(u64);
            if (offset == offsetof(DC_Header, m_textSize) || offset == state_script_entry_pointer_slot) {
                continue;
            }
            u64 old_value, new_value;
            std::memcpy(&old_value, old_raw + offset, sizeof(old_value));
            std::memcpy(&new_value, new_raw + offset, sizeof(new_value));
            const bool relocated = (old_reloc[slot / 8] & (1 << (slot % 8))) != 0;
            if (relocated && old_value >= old_strings_start) {
                EXPECT_EQ(new_value, old_value + string_delta) << "string pointer at offset 0x" << std::hex << offset;
            } else {
                EXPECT_EQ(new_value, old_value) << "data at offset 0x" << std::hex << offset;
            }
        }

        EXPECT_EQ(std::memcmp(new_raw + new_strings_start, old_raw + old_strings_start, old_text_size - old_strings_start), 0);

        Disassembler disassembler(&*patched, &full_sidbase);
        (void)disassembler.disassemble();
        ASSERT_TRUE(disassembler.has_state_script());
        EXPECT_EQ(disassembler.get_state_script()->m_declarations.size(), 31);

        const std::vector<const function_disassembly*> funcs = disassembler.get_named_functions();
        EXPECT_GE(funcs.size(), expected_function_count);

        std::unordered_set<std::string> patched_function_ids;
        const function_disassembly* replaced = nullptr;
        const function_disassembly* untouched = nullptr;
        for (const function_disassembly* func : funcs) {
            patched_function_ids.insert(func->get_id());
            if (func->get_id() == "wait-for-spawn@start@main@0") {
                replaced = func;
            } else if (func->get_id() == "spawn-lockbox@start@main@0") {
                untouched = func;
            }
        }
        for (const std::string& id : original_function_ids) {
            EXPECT_TRUE(patched_function_ids.contains(id)) << "function " << id << " missing from the patched binary";
        }
        ASSERT_NE(replaced, nullptr);
        EXPECT_EQ(replaced->m_lines.size(), 4);
        EXPECT_TRUE(symbol_table_contains_string(*replaced, "hello world"));
        ASSERT_NE(untouched, nullptr);
        EXPECT_FALSE(symbol_table_contains_string(*untouched, "hello world"));

        const std::string second_source =
            "u0 #wait-for-spawn@start@main@0() {\n"
            "    >> \"hello again\";\n"
            "}\n";

        compilation::global_state second_global{};
        const std::optional<std::vector<compilation::program_binary_element>> second_elements = compilation::run_compilation(second_source, second_global);
        ASSERT_TRUE(second_elements.has_value());

        const auto second_binary = compilation::disassemble_target(output, full_sidbase, *second_elements, second_global);
        ASSERT_TRUE(second_binary.has_value()) << second_binary.error();
        const auto& [second_bytes, second_size] = *second_binary;

        const std::filesystem::path second_output = std::filesystem::temp_directory_path() / "dconstruct-patch-twice-ss-hub-lockbox.bin";
        write_binary_file(second_output, second_bytes.get(), second_size);

        resstr<BinaryFile> repatched = BinaryFile::from_path(second_output);
        ASSERT_TRUE(repatched.has_value()) << repatched.error();

        const std::vector<entry_record> repatched_entries = read_entry_records(*repatched);
        ASSERT_EQ(repatched_entries.size(), original_entries.size());
        for (u64 i = 0; i < original_entries.size(); ++i) {
            EXPECT_EQ(repatched_entries[i].m_name, original_entries[i].m_name);
            EXPECT_EQ(repatched_entries[i].m_type, original_entries[i].m_type);
            if (original_entries[i].m_name != state_script_name) {
                EXPECT_EQ(repatched_entries[i].m_offset, original_entries[i].m_offset);
            }
        }

        Disassembler second_disassembler(&*repatched, &full_sidbase);
        (void)second_disassembler.disassemble();
        ASSERT_TRUE(second_disassembler.has_state_script());
        EXPECT_EQ(second_disassembler.get_state_script()->m_declarations.size(), 31);

        const std::vector<const function_disassembly*> second_funcs = second_disassembler.get_named_functions();
        EXPECT_EQ(second_funcs.size(), funcs.size());

        std::unordered_set<std::string> repatched_function_ids;
        const function_disassembly* second_replaced = nullptr;
        for (const function_disassembly* func : second_funcs) {
            repatched_function_ids.insert(func->get_id());
            if (func->get_id() == "wait-for-spawn@start@main@0") {
                second_replaced = func;
            }
        }
        for (const std::string& id : original_function_ids) {
            EXPECT_TRUE(repatched_function_ids.contains(id)) << "function " << id << " missing from the twice-patched binary";
        }
        ASSERT_NE(second_replaced, nullptr);
        EXPECT_TRUE(symbol_table_contains_string(*second_replaced, "hello again"));
        EXPECT_FALSE(symbol_table_contains_string(*second_replaced, "hello world"));

        std::filesystem::remove(output);
        std::filesystem::remove(second_output);
    }

    [[nodiscard]] static std::string normalized_asm_without_offsets(const std::filesystem::path& asm_path) {
        std::ifstream file(asm_path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        static const std::regex offset_pattern{"0x[0-9a-fA-F]+"};
        return std::regex_replace(buffer.str(), offset_pattern, "0x");
    }

    [[nodiscard]] static std::vector<std::string> split_asm_entry_sections(const std::string& listing) {
        std::vector<std::string> sections;
        const std::string marker = "##########  ENTRY";
        u64 header_start = listing.find(marker);
        while (header_start != std::string::npos) {
            u64 body_start = listing.find('\n', header_start);
            if (body_start == std::string::npos) {
                break;
            }
            ++body_start;
            const u64 next_header = listing.find(marker, body_start);
            const u64 body_end = next_header == std::string::npos ? listing.size() : listing.rfind('\n', next_header) + 1;
            sections.push_back(listing.substr(body_start, body_end - body_start));
            header_start = next_header;
        }
        return sections;
    }

    TEST(COMPILER, TargetPatchDisassemblyMatchesOriginal) {
        static SIDBase full_sidbase = *SIDBase::from_binary(TEST_DIR + "sidbase_new_fixed.bin");
        const std::filesystem::path target = TEST_ROOT / "fixtures" / "dc" / "ss-hub-lockbox.bin";

        const std::string source =
            "u0 #wait-for-spawn@start@main@0() {\n"
            "    >> \"hello world\";\n"
            "}\n";

        compilation::global_state global{};
        const std::optional<std::vector<compilation::program_binary_element>> elements = compilation::run_compilation(source, global);
        ASSERT_TRUE(elements.has_value());

        const auto binary = compilation::disassemble_target(target, full_sidbase, *elements, global);
        ASSERT_TRUE(binary.has_value()) << binary.error();
        const auto& [bytes, size] = *binary;

        const std::filesystem::path patched_bin = std::filesystem::temp_directory_path() / "dconstruct-disasm-patched-ss-hub-lockbox.bin";
        write_binary_file(patched_bin, bytes.get(), size);

        const std::filesystem::path original_asm = std::filesystem::temp_directory_path() / "dconstruct-disasm-original-ss-hub-lockbox.asm";
        const std::filesystem::path patched_asm = std::filesystem::temp_directory_path() / "dconstruct-disasm-patched-ss-hub-lockbox.asm";
        const std::unordered_map<sid64, ast::full_type> type_map;
        disassembly::disasm_file(target, original_asm, full_sidbase, type_map);
        disassembly::disasm_file(patched_bin, patched_asm, full_sidbase, type_map);

        const std::string original_listing = normalized_asm_without_offsets(original_asm);
        const std::string patched_listing = normalized_asm_without_offsets(patched_asm);
        const std::vector<std::string> original_sections = split_asm_entry_sections(original_listing);
        const std::vector<std::string> patched_sections = split_asm_entry_sections(patched_listing);
        ASSERT_EQ(original_sections.size(), 7);
        EXPECT_EQ(patched_sections.size(), original_sections.size());

        for (const std::string& section : original_sections) {
            const bool is_replaced_state_script = section.find("ss-hub-lockbox = state-script") != std::string::npos;
            const bool present = patched_listing.find(section) != std::string::npos;
            if (is_replaced_state_script) {
                EXPECT_FALSE(present) << "the replaced state script disassembles identically to the original";
            } else {
                EXPECT_TRUE(present) << "entry section missing from the patched disassembly:\n" << section.substr(0, 300);
            }
        }

        std::filesystem::remove(patched_bin);
        std::filesystem::remove(original_asm);
        std::filesystem::remove(patched_asm);
    }

}
