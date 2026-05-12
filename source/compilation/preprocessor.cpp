#include "compilation/preprocessor.h"

#include <fstream>
#include <sstream>
#include <string_view>
#include <unordered_set>

namespace dconstruct::compilation {

[[nodiscard]] static std::filesystem::path canonical_existing_path(const std::filesystem::path& path) noexcept {
    std::error_code ec;
    std::filesystem::path canonical = std::filesystem::weakly_canonical(path, ec);
    if (!ec) {
        return canonical;
    }
    std::filesystem::path absolute = std::filesystem::absolute(path, ec);
    if (!ec) {
        return absolute.lexically_normal();
    }
    return path.lexically_normal();
}

[[nodiscard]] static std::string location_error(const source_location& loc, std::string message) {
    return format_source_location(loc) + ": " + std::move(message);
}

[[nodiscard]] static std::string normalize_directive_line(std::string line) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    return line;
}

[[nodiscard]] static std::expected<std::filesystem::path, std::string> parse_quoted_path(
    const std::string& line,
    const u64 first_space,
    const source_location& loc) noexcept {

    const u64 first_quote = line.find('"', first_space + 1);
    const u64 last_quote = line.rfind('"');
    if (first_quote == std::string::npos || last_quote == std::string::npos || first_quote == last_quote || last_quote != line.size() - 1) {
        return std::unexpected{location_error(loc, "malformed precompiler directive path")};
    }
    return std::filesystem::path{std::string(line.data() + first_quote + 1, last_quote - first_quote - 1)};
}

[[nodiscard]] static std::optional<std::string> preprocess_dcpl(
    const std::string& source,
    const std::filesystem::path& source_path,
    compiler_options& options,
    std::unordered_set<std::string>& included_paths,
    std::string& stripped_source,
    std::vector<source_location>& line_map) noexcept {

    std::istringstream iss(source);
    const std::filesystem::path current_path = canonical_existing_path(source_path);
    const std::filesystem::path current_dir = current_path.parent_path();
    u32 line_number = 0;

    for (std::string raw_line; std::getline(iss, raw_line);) {
        ++line_number;
        const std::string line = normalize_directive_line(std::move(raw_line));
        const source_location loc{current_path, line_number};

        if (!line.starts_with("@")) {
            stripped_source += line;
            stripped_source += '\n';
            line_map.push_back(loc);
            continue;
        }

        if (line == "@standalone") {
            options.m_standalone = true;
            stripped_source += '\n';
            line_map.push_back(loc);
            continue;
        }

        const u64 first_space = line.find_first_of(' ', 1);
        if (first_space == std::string::npos) {
            return location_error(loc, "malformed precompiler directive");
        }

        const std::string_view directive_name(line.data() + 1, first_space - 1);
        std::expected<std::filesystem::path, std::string> path_res = parse_quoted_path(line, first_space, loc);
        if (!path_res) {
            return path_res.error();
        }
        const std::filesystem::path& path = *path_res;

        if (directive_name == "include") {
            if (path.is_absolute()) {
                return location_error(loc, "@include expects a relative path");
            }

            const std::filesystem::path include_path = canonical_existing_path(current_dir / path);
            std::error_code ec;
            if (!std::filesystem::exists(include_path, ec)) {
                return location_error(loc, "included file does not exist: " + include_path.string());
            }
            if (!included_paths.insert(include_path.string()).second) {
                stripped_source += '\n';
                line_map.push_back(loc);
                continue;
            }

            std::ifstream include_in{include_path};
            if (!include_in.is_open()) {
                return location_error(loc, "couldn't open included file " + include_path.string());
            }

            std::stringstream input;
            input << include_in.rdbuf();
            if (std::optional<std::string> err = preprocess_dcpl(input.str(), include_path, options, included_paths, stripped_source, line_map)) {
                return err;
            }
            continue;
        }

        stripped_source += '\n';
        line_map.push_back(loc);

        if (directive_name == "standalone") {
            return location_error(loc, "@standalone does not take a path");
        } else if (directive_name == "target") {
            options.m_target = path;
        } else if (directive_name == "output") {
            options.m_output = path;
        } else if (directive_name == "modules") {
            options.m_modules = path;
        } else if (directive_name == "sidbase") {
            options.m_sidbase = path;
        } else if (directive_name == "repackage") {
            options.m_repackage = path;
        } else {
            return location_error(loc, "unknown precompiler directive: " + std::string(directive_name));
        }
    }

    return std::nullopt;
}

[[nodiscard]] std::expected<compiler_options, std::string> compiler_options::from_args(const cxxopts::ParseResult& args) noexcept {
    std::filesystem::path target_binary_filepath;
    if (args.count("t") == 1) {
        target_binary_filepath = args["t"].as<std::string>();
    }

    std::filesystem::path output_filepath;
    if (args.count("o") == 1) {
       output_filepath = args["o"].as<std::string>();
    }

    std::filesystem::path modules_filepath;
    if (args.count("m") == 1) {
       output_filepath = args["m"].as<std::string>();
    }

    std::filesystem::path sidbase_filepath;
    if (args.count("s") == 1) {
       sidbase_filepath = args["s"].as<std::string>();
    }

    std::optional<std::filesystem::path> repackage_filepath;
    if (args.count("r") == 1) {
       repackage_filepath = args["r"].as<std::string>();
    }

    return compiler_options{std::move(target_binary_filepath), std::move(output_filepath), std::move(modules_filepath), std::move(sidbase_filepath), std::move(repackage_filepath)}; 
}

[[nodiscard]] std::expected<compiler_options, std::string> compiler_options::from_dcpl(
    std::string& source,
    const std::filesystem::path& source_path,
    std::vector<source_location>& line_map) noexcept {
    compiler_options out;
    std::string stripped_source;
    stripped_source.reserve(source.size());
    line_map.clear();

    std::unordered_set<std::string> included_paths;
    included_paths.insert(canonical_existing_path(source_path).string());
    if (std::optional<std::string> err = preprocess_dcpl(source, source_path, out, included_paths, stripped_source, line_map)) {
        return std::unexpected{std::move(*err)};
    }
    stripped_source.shrink_to_fit();

    source = std::move(stripped_source);
    return out;
}

[[nodiscard]] std::expected<std::filesystem::path, std::string> check_single_path_provided(const std::filesystem::path& lhs, const std::filesystem::path& rhs, std::string param_name, bool check_exists = true) noexcept {
    if (lhs.empty() && rhs.empty()) {
        return std::unexpected{"no precompiler directive or command line argument found for the required parameter '" + std::move(param_name) + '\''};
    } else if (!lhs.empty() && !rhs.empty()) {
        return std::unexpected{"provided both a command line argument and a precompiler directive for the option '" + std::move(param_name) + "\'. please only provide one or the other."};
    } else if (!lhs.empty()) {
        if (!std::filesystem::exists(lhs) && check_exists) {
            return std::unexpected{"expected file to exist but got missing path " + lhs.string()};
        } else {
            return lhs;
        }
    } else {
        if (!std::filesystem::exists(rhs) && check_exists) {
            return std::unexpected{"expected file to exist but got missing path " + rhs.string()};
        } else {
            return rhs;
        }
    }
}

[[nodiscard]] std::expected<compiler_options, std::string> compiler_options::parse(
    const cxxopts::ParseResult& args,
    std::string& source,
    const std::filesystem::path& source_path,
    std::vector<source_location>& line_map) noexcept {
    std::expected<compiler_options, std::string> from_args = compiler_options::from_args(args);
    if (!from_args) {
        return from_args;
    }
    std::expected<compiler_options, std::string> from_dcpl = compiler_options::from_dcpl(source, source_path, line_map);
    if (!from_dcpl) {
        return from_dcpl;
    }
    
    std::expected<std::filesystem::path, std::string> output_res = check_single_path_provided(from_args->m_output, from_dcpl->m_output, "output", false);
    if (!output_res) {
        return std::unexpected{std::move(output_res.error())};
    }

    const bool standalone = from_args->m_standalone || from_dcpl->m_standalone;

    std::filesystem::path target;
    if (!standalone) {
        std::expected<std::filesystem::path, std::string> target_res = check_single_path_provided(from_args->m_target, from_dcpl->m_target, "target");
        if (!target_res) {
            return std::unexpected{std::move(target_res.error())};
        }
        target = std::move(*target_res);
    } else if (!from_args->m_target.empty() && !from_dcpl->m_target.empty()) {
        return std::unexpected{"provided both a command line argument and a precompiler directive for the option 'target'. please only provide one or the other."};
    } else if (!from_args->m_target.empty()) {
        target = std::move(from_args->m_target);
    } else {
        target = std::move(from_dcpl->m_target);
    }

    std::filesystem::path modules;
    if (!standalone) {
        std::expected<std::filesystem::path, std::string> modules_res = check_single_path_provided(from_args->m_modules, from_dcpl->m_modules, "modules");
        if (!modules_res) {
            return std::unexpected{std::move(modules_res.error())};
        }
        modules = std::move(*modules_res);
    } else if (!from_args->m_modules.empty() || !from_dcpl->m_modules.empty()) {
        std::expected<std::filesystem::path, std::string> modules_res = check_single_path_provided(from_args->m_modules, from_dcpl->m_modules, "modules");
        if (!modules_res) {
            return std::unexpected{std::move(modules_res.error())};
        }
        modules = std::move(*modules_res);
    }

    std::expected<std::filesystem::path, std::string> sidbase_res = check_single_path_provided(from_args->m_sidbase, from_dcpl->m_sidbase, "sidbase");
    if (!sidbase_res) {
        return std::unexpected{std::move(sidbase_res.error())};
    }

    std::optional<std::filesystem::path> repackage_res = std::nullopt;
    if (from_args->m_repackage || from_dcpl->m_repackage) {
        const std::filesystem::path args_repackage = from_args->m_repackage.value_or(std::filesystem::path{});
        const std::filesystem::path dcpl_repackage = from_dcpl->m_repackage.value_or(std::filesystem::path{});
        std::expected<std::filesystem::path, std::string> repackage_exp = check_single_path_provided(args_repackage, dcpl_repackage, "repackage");
        if (!repackage_exp) {
            return std::unexpected{std::move(repackage_exp.error())};
        } else if (!std::filesystem::is_directory(*repackage_exp)) {
            return std::unexpected{"expected directory for repackage path but got non-directory"};
        }
        repackage_res = std::move(*repackage_exp);
    }
    

    return compiler_options{std::move(target), std::move(*output_res), std::move(modules), std::move(*sidbase_res), std::move(repackage_res), standalone};
}

[[nodiscard]] std::expected<compiler_options, std::string> compiler_options::parse(const cxxopts::ParseResult& args, std::string& source) noexcept {
    std::vector<source_location> line_map;
    return parse(args, source, {}, line_map);
}


}
