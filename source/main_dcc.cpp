#include "compilation/compiler_funcs.h"

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
        std::cerr << "error: no input specified\n";
        return -1;
    } else {
        source_code_filepath = opts["i"].as<std::string>();
    }
    std::ifstream file_in{source_code_filepath};
    if (!file_in.is_open()) {
        std::cerr << "error: couldn't open " << source_code_filepath << "\n";
        return -1;
    }

    std::stringstream input;
    input << file_in.rdbuf();

    std::string source = input.str();
    std::vector<dconstruct::compilation::source_location> line_map;

    const dconstruct::resstr<dconstruct::compilation::compiler_options> compiler_options_res =
        dconstruct::compilation::compiler_options::parse(opts, source, source_code_filepath, line_map);
    if (!compiler_options_res) {
        std::cerr << compiler_options_res.error() << "\n";
        return -1;
    }

    const dconstruct::resstr<dconstruct::SIDBase> sidbase_res = dconstruct::SIDBase::from_binary(compiler_options_res->m_sidbase);
    if (!sidbase_res) {
        std::cerr << sidbase_res.error() << "\n";
        return -1;
    }

    const dconstruct::SIDBase& sidbase = *sidbase_res;

    dconstruct::compilation::global_state global;

    const auto compile_res = dconstruct::compilation::run_compilation_detailed(source, global, line_map);
    if (!compile_res) {
        return -1;
    }

    const auto& functions = compile_res->m_elements;
    std::vector<dconstruct::compilation::pak68_edit_request> pak68_edits;
    if (!compile_res->m_stateScriptNames.empty() && !compiler_options_res->m_pak68) {
        std::cerr << "[warning] compiled state script";
        if (compile_res->m_stateScriptNames.size() == 1) {
            std::cerr << " " << compile_res->m_stateScriptNames.front();
        }
        std::cerr << " but no @mod path was provided; sp-all pak68 entry was not added\n";
    }
    if (compiler_options_res->m_pak68) {
        pak68_edits = dconstruct::compilation::pak68_edits_for_compile(*compiler_options_res, compile_res->m_stateScriptNames);
        if (const dconstruct::errmsg pak68_err =
                dconstruct::compilation::validate_pak68_edits(*compiler_options_res->m_pak68, pak68_edits)) {
            std::cerr << *pak68_err << "\n";
            return -1;
        }
    }

    const auto output_res = create_output(compiler_options_res, sidbase, functions, global);
    if (!output_res) {
        std::cerr << output_res.error() << "\n";
        return -1;
    }

    std::vector<dconstruct::compilation::pak68_edit_summary> pak68_summaries;
    if (compiler_options_res->m_pak68) {
        const dconstruct::resstr<std::vector<dconstruct::compilation::pak68_edit_summary>> pak68_res =
            dconstruct::compilation::apply_pak68_edits(*compiler_options_res->m_pak68, pak68_edits);
        if (!pak68_res) {
            std::cerr << pak68_res.error() << "\n";
            return -1;
        }
        pak68_summaries = std::move(*pak68_res);
    }

    std::optional<std::filesystem::path> psarc_output;
    if (compiler_options_res->m_repackage) {
        const dconstruct::resstr<std::filesystem::path> repackage_res = dconstruct::compilation::repackage_psarc(*compiler_options_res->m_repackage);
        if (!repackage_res) {
            std::cerr << repackage_res.error() << "\n";
            return -1;
        }
        psarc_output = *repackage_res;
    }

    std::cout << "Compile summary\n";
    std::cout << "  Compile time: " << compile_res->m_duration.count() << " ms\n";
    std::cout << "  Sidbase: " << compiler_options_res->m_sidbase << "\n";
    std::cout << "  Output binary: " << output_res->m_output << " (" << output_res->m_size << " bytes)\n";
    if (output_res->m_modulesPatch) {
        const auto& patch = *output_res->m_modulesPatch;
        std::cout << "  modules.bin: " << patch.m_path << " updated " << patch.m_targetName
                  << " size " << patch.m_oldSize << " -> " << patch.m_newSize
                  << " at offset 0x" << std::hex << patch.m_offset << std::dec << "\n";
    } else {
        std::cout << "  modules.bin: not changed\n";
    }
    if (psarc_output) {
        std::cout << "  PSARC output: " << *psarc_output << "\n";
    } else {
        std::cout << "  PSARC output: not produced\n";
    }
    if (pak68_summaries.empty()) {
        std::cout << "  pak68: not changed\n";
    } else {
        for (const auto& summary : pak68_summaries) {
            std::cout << "  pak68: " << summary.m_path << " level " << summary.m_levelName
                      << " added " << summary.m_added.size()
                      << ", already present " << summary.m_alreadyPresent.size() << "\n";
            for (const auto& entry : summary.m_added) {
                std::cout << "    + " << dconstruct::compilation::pak68_type_to_string(entry.m_type)
                          << " " << entry.m_sid << "\n";
            }
        }
    }
    return 0;
}
