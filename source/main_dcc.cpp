#include "compilation/compiler_funcs.h"

#include <print>

int main(int argc, char* argv[]) {
    // Qword(GetRegValue("RSP") + 0xC8 + 5*8)
    // GetString(Qword(GetRegValue("RSP") + 0xC8 + 5*8), -1, STRTYPE_C)

    const std::optional<cxxopts::ParseResult> options = dconstruct::compilation::get_command_line_options(argc, argv);
    if (!options) {
        return -1;
    }
    const cxxopts::ParseResult opts = *options;

    std::filesystem::path source_code_filepath;
    if (opts.count("i") == 0) {
        std::println(stderr, "error: no input specified");
        return -1;
    } else {
        source_code_filepath = opts["i"].as<std::string>();
    }
    std::ifstream file_in{source_code_filepath};
    if (!file_in.is_open()) {
        std::println(stderr, "error: couldn't open {}", source_code_filepath.string());
        return -1;
    }

    std::stringstream input;
    input << file_in.rdbuf();

    std::string source = input.str();
    std::vector<dconstruct::compilation::source_location> line_map;

    const dconstruct::resstr<dconstruct::compilation::compiler_options> compiler_options_res =
        dconstruct::compilation::compiler_options::parse(opts, source, source_code_filepath, line_map);
    if (!compiler_options_res) {
        std::println(stderr, "{}", compiler_options_res.error());
        return -1;
    }

    const dconstruct::resstr<dconstruct::SIDBase> sidbase_res = dconstruct::SIDBase::from_binary(compiler_options_res->m_sidbase);
    if (!sidbase_res) {
        std::println(stderr, "{}", sidbase_res.error());
        return -1;
    }

    const dconstruct::SIDBase& sidbase = *sidbase_res;

    if (const dconstruct::errmsg workspace_err = dconstruct::compilation::prepare_mod_workspace(*compiler_options_res)) {
        std::println(stderr, "{}", *workspace_err);
        return -1;
    }

    dconstruct::compilation::global_state global;

    const auto compile_res = dconstruct::compilation::run_compilation_detailed(source, global, line_map);
    if (!compile_res) {
        return -1;
    }

    const auto& functions = compile_res->m_elements;
    std::vector<dconstruct::compilation::pak68_edit_request> pak68_edits;
    if (!compile_res->m_stateScriptNames.empty() && !compiler_options_res->m_pak68) {
        std::string warning = "[warning] compiled state script";
        if (compile_res->m_stateScriptNames.size() == 1) {
            warning += " " + compile_res->m_stateScriptNames.front();
        }
        warning += " but no @mod path was provided; sp-all pak68 entry was not added";
        std::println(stderr, "{}", warning);
    }
    if (compiler_options_res->m_pak68) {
        pak68_edits = dconstruct::compilation::pak68_edits_for_compile(*compiler_options_res, compile_res->m_stateScriptNames);
        if (const dconstruct::errmsg pak68_err =
                dconstruct::compilation::validate_pak68_edits(*compiler_options_res->m_pak68, pak68_edits)) {
            std::println(stderr, "{}", *pak68_err);
            return -1;
        }
    }

    const auto output_res = create_output(compiler_options_res, sidbase, functions, global);
    if (!output_res) {
        std::println(stderr, "{}", output_res.error());
        return -1;
    }

    std::vector<dconstruct::compilation::pak68_edit_summary> pak68_summaries;
    if (compiler_options_res->m_pak68) {
        const dconstruct::resstr<std::vector<dconstruct::compilation::pak68_edit_summary>> pak68_res =
            dconstruct::compilation::apply_pak68_edits(*compiler_options_res->m_pak68, pak68_edits);
        if (!pak68_res) {
            std::println(stderr, "{}", pak68_res.error());
            return -1;
        }
        pak68_summaries = std::move(*pak68_res);
    }

    std::optional<std::filesystem::path> psarc_output;
    if (compiler_options_res->m_repackage) {
        const dconstruct::resstr<std::filesystem::path> repackage_res = dconstruct::compilation::repackage_psarc(*compiler_options_res->m_repackage);
        if (!repackage_res) {
            std::println(stderr, "{}", repackage_res.error());
            return -1;
        }
        psarc_output = *repackage_res;
    }

    std::println("Compile summary");
    std::println("  Compile time: {} ms", compile_res->m_duration.count());
    std::println("  Game: {}", compiler_options_res->m_game.string());
    std::println("  Mode: {}", compiler_options_res->m_standalone ? "standalone" : "target");
    if (!compiler_options_res->m_standalone) {
        std::println("  Target: {}", compiler_options_res->m_target.string());
    }
    std::println("  Sidbase: {}", compiler_options_res->m_sidbase.string());
    std::println("  Output binary: {} ({} bytes)", output_res->m_output.string(), output_res->m_size);
    if (output_res->m_modulesPatch) {
        const auto& patch = *output_res->m_modulesPatch;
        std::println("  modules.bin: {} updated {} size {} -> {} at offset {:#x}",
                     patch.m_path.string(), patch.m_targetName, patch.m_oldSize, patch.m_newSize, patch.m_offset);
    } else {
        std::println("  modules.bin: not changed");
    }
    if (psarc_output) {
        std::println("  PSARC output: {}", psarc_output->string());
    } else {
        std::println("  PSARC output: not produced");
    }
    if (pak68_summaries.empty()) {
        std::println("  pak68: not changed");
    } else {
        for (const auto& summary : pak68_summaries) {
            std::println("  pak68: {} level {} added {}, already present {}",
                         summary.m_path.string(), summary.m_levelName, summary.m_added.size(), summary.m_alreadyPresent.size());
            for (const auto& entry : summary.m_added) {
                std::println("    + {} {}", dconstruct::compilation::pak68_type_to_string(entry.m_type), entry.m_sid);
            }
        }
    }
    return 0;
}
