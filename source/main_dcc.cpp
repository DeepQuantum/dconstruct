#include "compilation/compiler_funcs.h"

#include <print>
#include <unordered_map>

namespace {

    struct compiler_job {
        std::filesystem::path m_sourcePath;
        std::string m_source;
        dconstruct::compilation::source_file_table m_sourceFiles;
        std::vector<dconstruct::compilation::source_location> m_lineMap;
        dconstruct::compilation::compiler_options m_options;
        dconstruct::compilation::global_state m_global;
        dconstruct::compilation::compilation_run_result m_compile;
        dconstruct::compilation::compiler_output_summary m_output;
    };

    [[nodiscard]] static std::filesystem::path path_key(const std::filesystem::path& path) {
        std::error_code ec;
        std::filesystem::path canonical = std::filesystem::weakly_canonical(path, ec);
        if (!ec) {
            return canonical;
        }
        return std::filesystem::absolute(path, ec).lexically_normal();
    }

    [[nodiscard]] static std::filesystem::path standalone_output_for_multi_input(const compiler_job& job) {
        std::filesystem::path output = job.m_options.m_output.parent_path() / job.m_sourcePath.filename();
        output.replace_extension(".bin");
        return output;
    }

    [[nodiscard]] static std::string compile_mode(const std::vector<compiler_job>& jobs) {
        bool has_standalone = false;
        bool has_target = false;
        for (const compiler_job& job : jobs) {
            has_standalone |= job.m_options.m_standalone;
            has_target |= !job.m_options.m_standalone;
        }
        if (has_standalone && has_target) {
            return "mixed";
        }
        return has_standalone ? "standalone" : "target";
    }

    [[nodiscard]] static dconstruct::resstr<std::vector<compiler_job>> parse_jobs(
        const cxxopts::ParseResult& opts,
        const std::vector<std::string>& input_paths
    ) {
        std::vector<compiler_job> jobs;
        jobs.reserve(input_paths.size());

        for (const std::string& input_path : input_paths) {
            compiler_job job;
            job.m_sourcePath = input_path;

            std::ifstream file_in{job.m_sourcePath};
            if (!file_in.is_open()) {
                return std::unexpected{"error: couldn't open " + job.m_sourcePath.string()};
            }

            std::stringstream input;
            input << file_in.rdbuf();
            job.m_source = input.str();

            dconstruct::resstr<dconstruct::compilation::compiler_options> options =
                dconstruct::compilation::compiler_options::parse(opts, job.m_source, job.m_sourcePath, job.m_sourceFiles, job.m_lineMap);
            if (!options) {
                return std::unexpected{options.error()};
            }
            job.m_options = std::move(*options);
            jobs.push_back(std::move(job));
        }

        if (jobs.size() > 1) {
            for (compiler_job& job : jobs) {
                if (job.m_options.m_standalone) {
                    job.m_options.m_output = standalone_output_for_multi_input(job);
                }
            }
        }

        const compiler_job& first = jobs.front();
        const std::filesystem::path game_key = path_key(first.m_options.m_game);
        const std::filesystem::path mod_key = path_key(*first.m_options.m_mod);

        std::unordered_map<std::string, std::filesystem::path> targets;
        std::unordered_map<std::string, std::filesystem::path> outputs;
        for (const compiler_job& job : jobs) {
            if (path_key(job.m_options.m_game) != game_key) {
                return std::unexpected{"all input files must use the same @game directory"};
            }
            if (!job.m_options.m_mod || path_key(*job.m_options.m_mod) != mod_key) {
                return std::unexpected{"all input files must use the same @mod directory"};
            }

            const std::string output_key = path_key(job.m_options.m_output).string();
            if (const auto [it, inserted] = outputs.emplace(output_key, job.m_sourcePath); !inserted) {
                return std::unexpected{"multiple inputs would write " + job.m_options.m_output.string() + ": " + it->second.string() + " and " + job.m_sourcePath.string()};
            }

            if (job.m_options.m_standalone) {
                continue;
            }
            const std::string target_key = path_key(job.m_options.m_target).string();
            if (const auto [it, inserted] = targets.emplace(target_key, job.m_sourcePath); !inserted) {
                return std::unexpected{"@target " + job.m_options.m_target.string() + " is used by both " + it->second.string() + " and " + job.m_sourcePath.string()};
            }
        }

        return jobs;
    }

}

int main(int argc, char* argv[]) {
    // Qword(GetRegValue("RSP") + 0xC8 + 5*8)
    // GetString(Qword(GetRegValue("RSP") + 0xC8 + 5*8), -1, STRTYPE_C)

    const std::optional<cxxopts::ParseResult> options = dconstruct::compilation::get_command_line_options(argc, argv);
    if (!options) {
        return -1;
    }
    const cxxopts::ParseResult opts = *options;

    if (opts.count("i") == 0) {
        std::println(stderr, "error: no input specified");
        return -1;
    }

    const std::vector<std::string> input_paths = opts["i"].as<std::vector<std::string>>();
    if (input_paths.empty()) {
        std::println(stderr, "error: no input specified");
        return -1;
    }

    dconstruct::resstr<std::vector<compiler_job>> jobs_res = parse_jobs(opts, input_paths);
    if (!jobs_res) {
        std::println(stderr, "{}", jobs_res.error());
        return -1;
    }
    std::vector<compiler_job> jobs = std::move(*jobs_res);
    dconstruct::compilation::compiler_options& common_options = jobs.front().m_options;

    const dconstruct::resstr<dconstruct::SIDBase> sidbase_res = dconstruct::SIDBase::from_binary(common_options.m_sidbase);
    if (!sidbase_res) {
        std::println(stderr, "{}", sidbase_res.error());
        return -1;
    }

    const dconstruct::SIDBase& sidbase = *sidbase_res;

    if (const dconstruct::errmsg workspace_err = dconstruct::compilation::prepare_mod_workspace(common_options)) {
        std::println(stderr, "{}", *workspace_err);
        return -1;
    }

    std::vector<dconstruct::compilation::pak68_edit_request> explicit_pak68_edits;
    std::vector<std::string> state_script_names;
    std::chrono::milliseconds compile_time{};

    for (compiler_job& job : jobs) {
        std::optional<dconstruct::compilation::compilation_run_result> compile_res =
            dconstruct::compilation::run_compilation_detailed(job.m_source, job.m_global, job.m_lineMap);
        if (!compile_res) {
            return -1;
        }
        compile_time += compile_res->m_duration;
        explicit_pak68_edits.insert(
            explicit_pak68_edits.end(),
            job.m_options.m_pak68Edits.begin(),
            job.m_options.m_pak68Edits.end()
        );
        state_script_names.insert(
            state_script_names.end(),
            compile_res->m_stateScriptNames.begin(),
            compile_res->m_stateScriptNames.end()
        );
        job.m_compile = std::move(*compile_res);
    }

    dconstruct::compilation::compiler_options pak68_options = common_options;
    pak68_options.m_pak68Edits = std::move(explicit_pak68_edits);

    std::vector<dconstruct::compilation::pak68_edit_request> pak68_edits;
    if (!state_script_names.empty() && !common_options.m_pak68) {
        std::println(stderr, "[warning] compiled state scripts but no @mod path was provided; sp-all pak68 entries were not added");
    }
    if (common_options.m_pak68) {
        pak68_edits = dconstruct::compilation::pak68_edits_for_compile(pak68_options, state_script_names);
        if (const dconstruct::errmsg pak68_err =
                dconstruct::compilation::validate_pak68_edits(*common_options.m_pak68, pak68_edits)) {
            std::println(stderr, "{}", *pak68_err);
            return -1;
        }
    }

    std::vector<dconstruct::compilation::modules_patch_request> modules_requests;
    modules_requests.reserve(jobs.size());
    for (compiler_job& job : jobs) {
        const auto output_res = create_output(job.m_options, sidbase, job.m_compile.m_elements, job.m_global);
        if (!output_res) {
            std::println(stderr, "{}", output_res.error());
            return -1;
        }
        job.m_output = std::move(*output_res);
        modules_requests.push_back(dconstruct::compilation::modules_patch_request{
            job.m_output.m_output,
            job.m_output.m_size,
            job.m_options.m_standalone
                ? dconstruct::compilation::module_exports_for_elements(job.m_compile.m_elements)
                : std::vector<sid64>{},
        });
    }

    dconstruct::resstr<std::vector<dconstruct::compilation::modules_patch_summary>> module_patches =
        dconstruct::compilation::patch_modules_sizes(common_options.m_modules, modules_requests);
    if (!module_patches) {
        std::println(stderr, "{}", module_patches.error());
        return -1;
    }
    for (u64 i = 0; i < jobs.size(); ++i) {
        jobs[i].m_output.m_modulesPatch = (*module_patches)[i];
    }

    std::vector<dconstruct::compilation::pak68_edit_summary> pak68_summaries;
    if (common_options.m_pak68) {
        const dconstruct::resstr<std::vector<dconstruct::compilation::pak68_edit_summary>> pak68_res =
            dconstruct::compilation::apply_pak68_edits(*common_options.m_pak68, pak68_edits);
        if (!pak68_res) {
            std::println(stderr, "{}", pak68_res.error());
            return -1;
        }
        pak68_summaries = std::move(*pak68_res);
    }

    std::optional<std::filesystem::path> psarc_output;
    if (common_options.m_repackage) {
        const dconstruct::resstr<std::filesystem::path> repackage_res = dconstruct::compilation::repackage_psarc(*common_options.m_repackage);
        if (!repackage_res) {
            std::println(stderr, "{}", repackage_res.error());
            return -1;
        }
        psarc_output = *repackage_res;
    }

    std::println("Compile summary");
    std::println("  Inputs: {}", jobs.size());
    std::println("  Compile time: {} ms", compile_time.count());
    std::println("  Game: {}", common_options.m_game.string());
    std::println("  Mode: {}", compile_mode(jobs));
    std::println("  Sidbase: {}", common_options.m_sidbase.string());
    for (const compiler_job& job : jobs) {
        std::println("  Input: {}", job.m_sourcePath.string());
        if (!job.m_options.m_standalone) {
            std::println("    Target: {}", job.m_options.m_target.string());
        }
        std::println("    Output binary: {} ({} bytes)", job.m_output.m_output.string(), job.m_output.m_size);
        if (job.m_output.m_modulesPatch) {
            const auto& patch = *job.m_output.m_modulesPatch;
            std::println("    modules.bin: {} updated {} size {} -> {} at offset {:#x}",
                         patch.m_path.string(), patch.m_targetName, patch.m_oldSize, patch.m_newSize, patch.m_offset);
        } else {
            std::println("    modules.bin: not changed");
        }
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
