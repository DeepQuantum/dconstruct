#include <gtest/gtest.h>
#include "compilation/lexer.h"
#include "compilation/dc_parser.h"
#include "compilation/compiler_funcs.h"
#include "compilation/preprocessor.h"
#include "compilation/function.h"
#include "disassembly/disassembler.h"
#include "disassembly/instructions.h"
#include "decompilation/decomp_function.h"
#include "binaryfile.h"
#include "ast/ast.h"
#include <vector>
#include <fstream>
#include <algorithm>
#include <filesystem>

namespace dconstruct::testing {


    
const std::filesystem::path TEST_ROOT = DCONSTRUCT_TEST_ROOT;
const std::string TEST_DIR = (TEST_ROOT / "fixtures" / "dc").string() + "/";
const std::string DCPL_PATH = (TEST_ROOT / "fixtures" / "dcpl").string() + "/";

    static SIDBase base = *SIDBase::from_binary(TEST_DIR + R"(\test_sidbase.bin)");

    static std::pair<std::vector<compilation::token>, std::vector<compilation::lexing_error>> get_tokens(const std::string &string) {
        dconstruct::compilation::Lexer lexer = dconstruct::compilation::Lexer(string);
        return { lexer.scan_tokens(), lexer.get_errors() };
    } 

    static cxxopts::ParseResult get_empty_options() {
        cxxopts::Options options("compiler_test", "");
        const char* argv[] = {"compiler_test"};
        return options.parse(1, argv);
    }

    static std::string dcpl_prelude() {
        return "@output \"test/fixtures/compiler/include_out.bin\"\n@sidbase \"" + TEST_DIR + "test_sidbase.bin\"\n";
    }

    static std::tuple<ast::program, std::unordered_map<std::string, ast::full_type>, std::vector<compilation::parsing_error>> get_parse_results(const std::vector<compilation::token> &tokens) {
        compilation::Parser parser{tokens};
        return { parser.parse(), parser.get_known_types(), parser.get_errors() };
    }

    static std::pair<std::list<stmnt_uptr>, std::vector<compilation::parsing_error>> get_statements(std::vector<compilation::token> &tokens) {
        if (tokens.back().m_type == compilation::token_type::_EOF) {
            tokens.pop_back();
        }

        std::vector<compilation::token> function_def_tokens {
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

        return { !program.m_declarations.empty() ? std::move(static_cast<ast::function_definition*>(program.m_declarations[0].get())->m_body.m_statements) : std::list<stmnt_uptr>{}, errors };
    }


    [[nodiscard]] static std::expected<std::vector<compilation::function>, std::string> compile_to_functions(const std::string& code) {
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
        for (const auto& [name, sid_literal] : scope.m_sidAliases) {
            const ast::full_type* type = scope.lookup(name);
            global.m_sidAliases.emplace(name, std::pair{*type, sid_literal.first});
        }
        std::vector<compilation::function> functions;
        for (const auto& decl : program.m_declarations) {
            auto* func_def = dynamic_cast<const ast::function_definition*>(decl.get());
            if (!func_def) {
                continue;
            }
            compilation::function fn{};
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
            if (fn.m_returnBranchLocations.empty()) {
                fn.emit_instruction(Opcode::Return, 0_r, 0_r);
            } else if (fn.m_returnBranchLocations.back() == fn.m_instructions.size() - 2) {
                fn.m_instructions.back() = Instruction(Opcode::Return, 0_r, 0_r);
                fn.m_returnBranchLocations.pop_back();
            }
            const u16 return_location = fn.m_instructions.size() - 1;
            for (const u64 branch_location : fn.m_returnBranchLocations) {
                Instruction& branch = fn.m_instructions[branch_location + 1];
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
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

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
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

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
            compilation::lexing_error(1, "invalid token '@'")
        };

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
            return std::ranges::count_if(tokens, [&](const compilation::token& token) {
                return token.m_type == compilation::token_type::IDENTIFIER && token.m_lexeme == name;
            });
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
        const std::filesystem::path mod_path = "test/fixtures/compiler/pak68_macro_mod";
        const std::filesystem::path pak_path = mod_path / "pak68_macro_mod-pak68.txt";
        std::filesystem::create_directories(pak_path.parent_path());
        {
            std::ofstream out{pak_path};
            out << "level-name sp-all\n"
                << "symbol existing-entry\n"
                << "level-name other-level\n"
                << "actor other-actor\n";
        }

        std::string source =
            "@mod \"test/fixtures/compiler/pak68_macro_mod\"\n"
            "@output \"pak68-macro\"\n"
            "@sidbase \"" + TEST_DIR + "test_sidbase.bin\"\n"
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
        const std::filesystem::path mod_path = "test/fixtures/compiler/pak68_unknown_type_mod";
        std::filesystem::create_directories(mod_path);

        std::string source =
            "@mod \"test/fixtures/compiler/pak68_unknown_type_mod\"\n"
            "@output \"pak68-unknown-type\"\n"
            "@sidbase \"" + TEST_DIR + "test_sidbase.bin\"\n"
            "@add_pak level-name sp-all { nope #gas-mask-ellie }\n"
            "u32 main() { return 0; }\n";
        std::vector<compilation::source_location> line_map;

        auto options = compilation::compiler_options::parse(get_empty_options(), source, "test/fixtures/compiler/pak68_unknown_type_test.dcpl", line_map);
        ASSERT_FALSE(options);
        EXPECT_NE(options.error().find("unknown pak68 type: nope"), std::string::npos);
    }

    TEST(COMPILER, AddPakCreatesMissingLevel) {
        const std::filesystem::path mod_path = "test/fixtures/compiler/pak68_missing_level_mod";
        const std::filesystem::path pak_path = mod_path / "pak68_missing_level_mod-pak68.txt";
        std::filesystem::create_directories(mod_path);
        std::filesystem::remove(pak_path);

        std::string source =
            "@mod \"test/fixtures/compiler/pak68_missing_level_mod\"\n"
            "@output \"pak68-missing-level\"\n"
            "@sidbase \"" + TEST_DIR + "test_sidbase.bin\"\n"
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

    TEST(COMPILER, ModDerivesRelativeOutputModulesPakAndRepackage) {
        const std::filesystem::path mod_path = "test/fixtures/compiler/derived_mod";
        std::filesystem::create_directories(mod_path / "bin" / "dc1");
        {
            std::ofstream modules{mod_path / "bin" / "dc1" / "modules.bin", std::ios::binary};
            modules << "placeholder";
        }
        {
            std::ofstream pak{mod_path / "pak68.txt"};
            pak << "level-name sp-all\n";
        }

        std::string source =
            "@mod \"test/fixtures/compiler/derived_mod\"\n"
            "@output \"ss-rogue/test-script-qntm\"\n"
            "@sidbase \"" + TEST_DIR + "test_sidbase.bin\"\n"
            "u32 main() { return 0; }\n";
        std::vector<compilation::source_location> line_map;

        auto options = compilation::compiler_options::parse(get_empty_options(), source, "test/fixtures/compiler/derived_mod_test.dcpl", line_map);
        ASSERT_TRUE(options) << options.error();
        EXPECT_TRUE(options->m_standalone);
        EXPECT_EQ(options->m_output, mod_path / "bin" / "dc1" / "ss-rogue" / "test-script-qntm.bin");
        EXPECT_EQ(options->m_modules, mod_path / "bin" / "dc1" / "modules.bin");
        ASSERT_TRUE(options->m_pak68);
        EXPECT_EQ(*options->m_pak68, mod_path / "derived_mod-pak68.txt");
        ASSERT_TRUE(options->m_repackage);
        EXPECT_EQ(*options->m_repackage, mod_path);
    }

    TEST(COMPILER, RepackagePathUsesExactModFolderName) {
        EXPECT_EQ(
            compilation::repackage_psarc_path("test/fixtures/compiler/derived_mod"),
            std::filesystem::path{"test/fixtures/compiler/derived_mod.psarc"});
        EXPECT_EQ(
            compilation::repackage_psarc_path("test/fixtures/compiler/derived_mod_unpacked"),
            std::filesystem::path{"test/fixtures/compiler/derived_mod_unpacked.psarc"});
    }

    TEST(COMPILER, ModOutputRootDirectoryRelativeToMod) {
        const std::filesystem::path mod_path = "test/fixtures/compiler/rooted_output_mod";
        std::filesystem::create_directories(mod_path);

        std::string source =
            "@mod \"test/fixtures/compiler/rooted_output_mod\"\n"
            "@output \"/bin/dc1/script-callbacks\"\n"
            "@sidbase \"" + TEST_DIR + "test_sidbase.bin\"\n"
            "u32 main() { return 0; }\n";
        std::vector<compilation::source_location> line_map;

        auto options = compilation::compiler_options::parse(get_empty_options(), source, "test/fixtures/compiler/rooted_output_mod_test.dcpl", line_map);
        ASSERT_TRUE(options) << options.error();
        EXPECT_EQ(options->m_output, mod_path / "bin" / "dc1" / "script-callbacks.bin");

        std::string source_without_leading_slash =
            "@mod \"test/fixtures/compiler/rooted_output_mod\"\n"
            "@output \"bin/dc1/script-callbacks\"\n"
            "@sidbase \"" + TEST_DIR + "test_sidbase.bin\"\n"
            "u32 main() { return 0; }\n";
        line_map.clear();

        options = compilation::compiler_options::parse(get_empty_options(), source_without_leading_slash, "test/fixtures/compiler/rooted_output_mod_test.dcpl", line_map);
        ASSERT_TRUE(options) << options.error();
        EXPECT_EQ(options->m_output, mod_path / "bin" / "dc1" / "script-callbacks.bin");
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
            {}
        });

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
            compilation::lexing_error(1, "expected '\"' to close string literal but got end of file")
        };

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
            compilation::token(compilation::token_type::IDENTIFIER, "int", 0, 3),
            compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 3),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 3),
            compilation::token(compilation::token_type::INT, "0", 0, 3),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 3),
            compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 4),
            compilation::token(compilation::token_type::PLUS_EQUAL, "+=", 0, 4),
            compilation::token(compilation::token_type::INT, "1", 0, 4),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 4),
            compilation::token(compilation::token_type::RETURN, "return", 0, 5),
            compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 5),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 5),
            compilation::token(compilation::token_type::RIGHT_BRACE, "}", 0, 6),
            compilation::token(compilation::token_type::_EOF, "", 0, 6),
        };
    }

    TEST(COMPILER, LexerComplexSids) {
        const std::string chars = "#simple_sid #%alloc-array? #=f(test123)";
        const auto [tokens, errors] = get_tokens(chars);
        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::SID, "#simple_sid", 0, 1),
            compilation::token(compilation::token_type::SID, "#%alloc-array?", 0, 1),
            compilation::token(compilation::token_type::SID, "#=f", 0, 1),
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
            "};"
            ;

        const std::vector<compilation::token> expected = {
            compilation::token(compilation::token_type::STRUCT, "struct", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "Person", 0, 1),
            compilation::token(compilation::token_type::LEFT_BRACE, "{", 0, 1),

            compilation::token(compilation::token_type::IDENTIFIER, "sid", 0, 2),
            compilation::token(compilation::token_type::IDENTIFIER, "name", 0, 2),
            compilation::token(compilation::token_type::EQUAL, "=", 0, 2),
            compilation::token(compilation::token_type::SID, "#ellie", 0, 2),
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
            compilation::token(compilation::token_type::_EOF, "", 0, 1) 
        };

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
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const ast::expression& actual = *dynamic_cast<const ast::expression_stmt*>(statements.front().get())->m_expression;
        
        const ast::add_expr expected{
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            std::make_unique<ast::literal>(1),
            std::make_unique<ast::literal>(2)
        };

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
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

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
            std::move(left)
        };

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
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

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
            std::make_unique<ast::literal>(5)
        };

        EXPECT_EQ(statements.size(), 1);
        EXPECT_EQ(errors.size(), 0);
        EXPECT_EQ(expected, actual);
    }

    TEST(COMPILER, VariableDeclaration0) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::IDENTIFIER, "u16", 0, 1),
            compilation::token(compilation::token_type::IDENTIFIER, "number", 0, 1),
            compilation::token(compilation::token_type::SEMICOLON, ";", 0, 1),
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

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
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

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
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const ast::block& actual = *dynamic_cast<const ast::block*>(statements.front().get());

        std::list<stmnt_uptr> expected_statements{};
        expected_statements.push_back(std::move(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::identifier>(compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 1))
        )));

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
            compilation::token(compilation::token_type::_EOF, "", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        const ast::block& actual = *dynamic_cast<const ast::block*>(statements.front().get());

        std::list<stmnt_uptr> expected_statements{};
        expected_statements.push_back(std::move(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::identifier>(compilation::token(compilation::token_type::IDENTIFIER, "a", 0, 1))
        )));

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
                )
            )
        ));
        expected.push_back(std::make_unique<ast::variable_declaration>(ast::make_type_from_prim(ast::primitive_kind::I32), "x", (u16)0));
        expected.push_back(std::make_unique<ast::variable_declaration>(ast::make_type_from_prim(ast::primitive_kind::I32), "y", (u16)1));
        expected.push_back(std::make_unique<ast::while_stmt>(
            std::make_unique<ast::compare_expr>(
                compilation::token(compilation::token_type::LESS, "<", 0, 1),
                std::make_unique<ast::identifier>("x"),
                std::make_unique<ast::identifier>("y")
            ),
            std::make_unique<ast::block>(
                std::move(block_stmnt)
            )
        ));

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

        expected.push_back(std::make_unique<ast::variable_declaration>(ast::make_type_from_prim(ast::primitive_kind::U32), "x",
            std::make_unique<ast::sub_expr>(
                compilation::token(compilation::token_type::MINUS, "-", 0, 1),
                std::make_unique<ast::add_expr>(
                    compilation::token(compilation::token_type::PLUS, "+", 0, 1),
                    std::make_unique<ast::literal>((u16)1),
                    std::make_unique<ast::mul_expr>(
                        compilation::token(compilation::token_type::STAR, "*", 0, 1),
                        std::make_unique<ast::literal>((u16)2),
                        std::make_unique<ast::literal>((u16)3)
                    )
                ),
                std::make_unique<ast::div_expr>(
                    compilation::token(compilation::token_type::SLASH, "/", 0, 1),
                    std::make_unique<ast::literal>((u16)4),
                    std::make_unique<ast::literal>((u16)5)
                )
            )
        ));

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
                )
            )
        ));
        expected.push_back(std::make_unique<ast::foreach_stmt>(
            ast::parameter{ast::make_type_from_prim(ast::primitive_kind::U32),"item",},
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
            )
        ));

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
            )
        ));

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
            )
        ));

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
        display_args.push_back(std::make_unique<ast::literal>(19));

        std::list<stmnt_uptr> expected;
        expected.push_back(std::make_unique<ast::expression_stmt>(
            std::make_unique<ast::call_expr>(
                compilation::token(compilation::token_type::GREATER_GREATER, ">>", 0, 1),
                std::make_unique<ast::sid_identifier>("#display"),
                std::move(display_args)
            )
        ));

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
                )
            )
        );

        ast::if_stmt expected_if(
            std::make_unique<ast::literal>((u16)1),
            std::make_unique<ast::block>(
                std::move(block)
            )
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
            compilation::lexing_error(1, "invalid token '@'")
        };

        EXPECT_EQ(tokens, expected);
        EXPECT_EQ(expected_errors, errors);
    }

    TEST(COMPILER, ParseError1) {
        std::vector<compilation::token> tokens = {
            compilation::token(compilation::token_type::INT, "1", 1, 1),
            compilation::token(compilation::token_type::PLUS, "+", 0, 1),
            compilation::token(compilation::token_type::_EOF, ";", 0, 1)
        };

        const auto [statements, errors] = get_statements(tokens);

        ASSERT_EQ(errors.size(), 2);

        const std::list<stmnt_uptr> expected_statements = {
        };
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
        expected_type.m_members["x"] = std::move(std::make_shared<ast::full_type>(ast::make_type_from_prim(ast::primitive_kind::F32)));
        expected_type.m_members["y"] = std::move(std::make_shared<ast::full_type>(ast::make_type_from_prim(ast::primitive_kind::F32)));
        expected_type.m_members["z"] = std::move(std::make_shared<ast::full_type>(ast::make_type_from_prim(ast::primitive_kind::F32)));
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
        EXPECT_EQ(access->to_c_string(), "Keycodes.SPACE");
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
        const std::string code = "struct Vector3 { f32 x; f32 y; f32 z; } enum Opcode { MOVE, LOAD, PUSH } i32 vector3_dist(Vector3 a) { return 1.0; }";
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
        "using #5445173390656D6D as near (string, u32, u32) -> string sprintf;"
        "u32 main() {"
        "    string message = sprintf(\"Hello World from DC version %d.%d\", 0, 0);"
        "    display(message, 19);"
        "    return 0;"
        "}";
        auto [tokens, lex_errors] = get_tokens(code);
        const auto [program, types, parse_errors] = get_parse_results(tokens);
        EXPECT_EQ(lex_errors.size(), 0);
        EXPECT_EQ(parse_errors.size(), 0);
        EXPECT_EQ(program.m_declarations.size(), 3);

        compilation::scope scope{types};
        std::vector<ast::semantic_check_error> semantic_errors = program.check_semantics(scope);
        ASSERT_EQ(scope.m_sidAliases.at("sprintf").first, 0x5445173390656D6D);

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
        ASSERT_TRUE(std::holds_alternative<ast::function_type>(using_decl->m_newIdentifier.m_type));

        const auto& format_type = std::get<ast::function_type>(using_decl->m_newIdentifier.m_type);
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
        expect_instructions("i32 main() { return (i32)1.0; }", {
            Instruction{Opcode::LoadStaticFloatImm, 0, 0, 0},
            Instruction{Opcode::CastInteger, 1, 0, 0},
            Instruction{Opcode::Move, 0, 1, 0},
            Instruction{Opcode::Return, 0, 0, 0_r},
        });
    }

    TEST(COMPILER, Load1) {
        expect_instructions("i32 main(i32* p) { return *p; }", {
            Instruction{Opcode::Move, 0, 49, 0},
            Instruction{Opcode::LoadI32, 1, 0, 0},
            Instruction{Opcode::Move, 0, 1, 0},
            Instruction{Opcode::Return, 0, 0, 0},
        });
    }

    TEST(COMPILER, Store1) {
        expect_instructions("u16 main(i32* p) { *p = (i32)42; return 0; }", {
            Instruction{Opcode::Move, 0, 49, 0},
            Instruction{Opcode::LoadU16Imm, 1, 42, 0},
            Instruction{Opcode::StoreI32, 2, 0, 1},
            Instruction{Opcode::LoadU16Imm, 2, 0, 0},
            Instruction{Opcode::Move, 0, 2, 0},
            Instruction{Opcode::Return, 0, 0, 0},
        });
    }

    TEST(COMPILER, Load2) {
        expect_instructions("u64 main(u64* p) { return *p; }", {
            Instruction{Opcode::Move, 0, 49, 0},
            Instruction{Opcode::LoadU64, 1, 0, 0},
            Instruction{Opcode::Move, 0, 1, 0},
            Instruction{Opcode::Return, 0, 0, 0},
        });
    }

    TEST(COMPILER, Store2) {
        expect_instructions("u16 main(u64* p) { *p = 42; return 0; }", {
            Instruction{Opcode::Move, 0, 49, 0},
            Instruction{Opcode::LoadU16Imm, 1, 42, 0},
            Instruction{Opcode::StoreU64, 2, 0, 1},
            Instruction{Opcode::LoadU16Imm, 2, 0, 0},
            Instruction{Opcode::Move, 0, 2, 0},
            Instruction{Opcode::Return, 0, 0, 0},
        });
    }

    TEST(COMPILER, ReturnLiteral) {
        expect_instructions("i32 main() { return 1; }", {
            Instruction{Opcode::LoadU16Imm, 0, 1, 0},
            Instruction{Opcode::Return, 0, 0, 0},
        });
    }

    TEST(COMPILER, ReturnNullLoadsZero) {
        expect_instructions("u64 main() { return null; }", {
            Instruction{Opcode::LoadU16Imm, 0, 0, 0},
            Instruction{Opcode::Return, 0, 0, 0},
        });
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

        const compilation::function& fn = (*functions)[0];
        EXPECT_EQ(fn.m_instructions, (std::vector<Instruction>{
            Instruction{Opcode::LoadStaticPointerImm, 0, 0, 0},
            Instruction{Opcode::LoadU16Imm, 1, 0, 0},
            Instruction{Opcode::IAddImm, 1, 1, 1},
            Instruction{Opcode::LoadStaticInt, 2, 1, 0},
            Instruction{Opcode::Move, 0, 2, 0},
            Instruction{Opcode::Return, 0, 0, 0},
        }));

        ASSERT_EQ(fn.m_symbolTable.size(), 3);
        EXPECT_EQ(fn.m_symbolTable[0], 0);
        EXPECT_EQ(fn.m_symbolTable[1], SID("ellie"));
        EXPECT_EQ(fn.m_symbolTable[2], SID("dina"));

        ASSERT_EQ(fn.m_symbolTableEntryPointers.size(), 3);
        EXPECT_EQ(fn.m_symbolTableEntryPointers[0], compilation::function::SYMBOL_TABLE_POINTER_KIND::STRING);
        EXPECT_EQ(fn.m_symbolTableEntryPointers[1], compilation::function::SYMBOL_TABLE_POINTER_KIND::NONE);
        EXPECT_EQ(fn.m_symbolTableEntryPointers[2], compilation::function::SYMBOL_TABLE_POINTER_KIND::NONE);
    }

    TEST(COMPILER, MatchArrayCompilationElseLoadsDefault) {
        const std::string code =
            "u64 main(u64 a) {"
            "   return match (a) { 1 -> #ellie, 3 -> #dina, else -> #joel };"
            "}";

        auto functions = compile_to_functions(code);
        ASSERT_TRUE(functions) << functions.error();
        ASSERT_EQ(functions->size(), 1);

        std::vector<Instruction> expected{
            Instruction{Opcode::Move, 0, 49, 0},
            Instruction{Opcode::QEX_InRangeI, 1, 0, 3},
            Instruction{Opcode::BranchIfNot, 6, 1, 0},
            Instruction{Opcode::IAddImm, 0, 0, 1},
            Instruction{Opcode::LoadStaticInt, 1, 0, 0},
            Instruction{Opcode::Branch, 7, 0, 0},
            Instruction{Opcode::LoadStaticInt, 1, 1, 0},
            Instruction{Opcode::Move, 0, 1, 0},
            Instruction{Opcode::Return, 0, 0, 0},
        };
        const compilation::function& fn = (*functions)[0];
        EXPECT_EQ(fn.m_instructions, expected);

        ASSERT_EQ(fn.m_symbolTable.size(), 5);
        EXPECT_EQ(fn.m_symbolTable[0], SID("ellie"));
        EXPECT_EQ(fn.m_symbolTable[1], SID("joel"));
        EXPECT_EQ(fn.m_symbolTable[2], SID("dina"));
        EXPECT_EQ(fn.m_symbolTable[3], 1);
        EXPECT_EQ(fn.m_symbolTable[4], 3);
    }

    TEST(COMPILER, IfElseReturn) {
        expect_instructions("i32 main() { if (1) { return 2; } return 3; }", {
            Instruction{Opcode::LoadU16Imm, 0, 1, 0},
            Instruction{Opcode::BranchIfNot, 4, 0, 0},
            Instruction{Opcode::LoadU16Imm, 0, 2, 0},
            Instruction{Opcode::Branch, 6, 0, 0},
            Instruction{Opcode::LoadU16Imm, 1, 3, 0},
            Instruction{Opcode::Move, 0, 1, 0},
            Instruction{Opcode::Return, 0, 0, 0},
        });
    }

    TEST(COMPILER, WhileLoop) {
        expect_instructions("i32 main() { i32 x = 0; i32 y = 1; while (x < y) { x = x + y; } return x; }", {
            Instruction{Opcode::LoadU16Imm, 0, 0, 0},
            Instruction{Opcode::LoadU16Imm, 1, 1, 0},
            Instruction{Opcode::ILessThan, 2, 0, 1},
            Instruction{Opcode::BranchIfNot, 6, 2, 0},
            Instruction{Opcode::IAdd, 0, 0, 1},
            Instruction{Opcode::Branch, 2, 0, 0},
            Instruction{Opcode::Return, 0, 0, 0},
        });
    }

    TEST(COMPILER, LogicalAndShortCircuit) {
        auto functions = compile_to_functions("i32 main() { if (0 && 42) { return 1; } return 0; }");
        ASSERT_TRUE(functions) << functions.error();
        ASSERT_FALSE(functions->empty());

        const auto& instructions = (*functions)[0].m_instructions;
        ASSERT_FALSE(instructions.empty());

        const auto lhs_branch_it = std::find_if(instructions.begin(), instructions.end(), [](const Instruction& instruction) {
            return instruction.opcode == Opcode::BranchIfNot;
        });
        ASSERT_NE(lhs_branch_it, instructions.end());

        const auto rhs_load_it = std::find_if(instructions.begin(), instructions.end(), [](const Instruction& instruction) {
            return instruction.opcode == Opcode::LoadU16Imm && instruction.operand1 == 42;
        });
        ASSERT_NE(rhs_load_it, instructions.end());

        const u16 lhs_branch_target = lhs_branch_it->destination | (lhs_branch_it->operand2 << 8);
        const u16 rhs_load_index = static_cast<u16>(std::distance(instructions.begin(), rhs_load_it));
        const u16 lhs_branch_index = static_cast<u16>(std::distance(instructions.begin(), lhs_branch_it));

        EXPECT_LT(lhs_branch_index, rhs_load_index);
        EXPECT_GT(lhs_branch_target, rhs_load_index);
        EXPECT_EQ(instructions.back().opcode, Opcode::Return);
    }

    TEST(COMPILER, IfLogicalAndBranchesDirectly) {
        expect_instructions("i32 main(i32 a, i32 b) { if (a && b) { return 1; } return 0; }", {
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
        });
    }

    TEST(COMPILER, IfLogicalOrBranchesDirectly) {
        expect_instructions("i32 main(i32 a, i32 b) { if (a || b) { return 1; } return 0; }", {
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
        });
    }

    TEST(COMPILER, IfGroupedLogicalBranchesDirectly) {
        expect_instructions("i32 main(i32 a, i32 b) { if ((a && b)) { return 1; } return 0; }", {
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
        });
    }

    TEST(COMPILER, Subscript1) {
        expect_instructions("i32 main(u16* bytes) { u16 my_byte = bytes[2]; }", {
            Instruction{Opcode::Move, 0_r, 49_r},
            Instruction{Opcode::LoadU16Imm, 2_r, 2_r, 0_r},
            Instruction{Opcode::IMulImm, 1_r, 2_r, 2_imm},
            Instruction{Opcode::IAdd, 1_r, 0_r, 1_r},
            Instruction{Opcode::LoadU16, 1_r, 1_r},
            Instruction{Opcode::Return, 0, 0, 0},
        });
    }

    TEST(COMPILER, Subscript2) {
        expect_instructions("i32 main(f32* floats) { floats[2] = 0.69; }", {
            Instruction{Opcode::Move, 0_r, 49_r},
            Instruction{Opcode::LoadU16Imm, 1_r, 2_r, 0_r},
            Instruction{Opcode::IMulImm, 2_r, 1_r, 4_imm},
            Instruction{Opcode::IAdd, 2_r, 0_r, 2_r},
            Instruction{Opcode::LoadStaticFloatImm, 1_r, 0, 0},
            Instruction{Opcode::StoreFloat, 3_r, 2_r, 1_r},
            Instruction{Opcode::Return, 0, 0, 0},
        });
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
        ASSERT_EQ((*functions)[0].m_instructions.size(), 3);
        EXPECT_EQ((*functions)[0].m_instructions[0].opcode, Opcode::LoadU16Imm);
        EXPECT_EQ((*functions)[0].m_instructions[0].operand1, 4);
        EXPECT_EQ((*functions)[0].m_instructions[0].operand2, 0);
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

}
