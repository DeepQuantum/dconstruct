#include "compilation/preprocessor.h"

#include <cctype>
#include <fstream>
#include <sstream>
#include <string_view>
#include <unordered_set>
#include <vector>

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

[[nodiscard]] static std::string_view trim(std::string_view value) noexcept {
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front()))) {
        value.remove_prefix(1);
    }
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back()))) {
        value.remove_suffix(1);
    }
    return value;
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

[[nodiscard]] static std::expected<pak68_entry, std::string> parse_pak68_entry(
    const std::string_view line,
    const source_location& loc) noexcept {

    const std::string_view trimmed_line = trim(line);
    std::istringstream iss{std::string{trimmed_line}};
    std::string type_name;
    std::string sid_name;
    std::string extra;
    iss >> type_name >> sid_name >> extra;

    if (type_name.empty() && sid_name.empty()) {
        return std::unexpected{location_error(loc, "empty @add_pak entry")};
    }
    if (type_name.empty() || sid_name.empty() || !extra.empty()) {
        return std::unexpected{location_error(loc, "malformed @add_pak entry; expected '<type> #sid'")};
    }

    const std::optional<pak68_type> type = pak68_type_from_string(type_name);
    if (!type) {
        return std::unexpected{location_error(loc, "unknown pak68 type: " + type_name)};
    }

    if (sid_name.front() == '#') {
        sid_name.erase(sid_name.begin());
    }
    if (sid_name.empty()) {
        return std::unexpected{location_error(loc, "malformed @add_pak entry; expected non-empty #sid")};
    }

    return pak68_entry{*type, std::move(sid_name)};
}

struct add_pak_header {
    std::string m_categoryType;
    std::string m_categoryName;
    std::string_view m_afterOpen;
};

[[nodiscard]] static std::expected<add_pak_header, std::string> parse_add_pak_header(
    const std::string& line,
    const source_location& loc) noexcept {

    constexpr std::string_view directive = "@add_pak";
    std::string_view rest{line};
    rest.remove_prefix(directive.size());
    rest = trim(rest);

    const u64 open_brace = rest.find('{');
    if (open_brace == std::string_view::npos) {
        return std::unexpected{location_error(loc, "malformed @add_pak macro; expected '{'")};
    }

    const std::string_view header = trim(rest.substr(0, open_brace));
    std::istringstream header_iss{std::string{header}};
    std::string category_type;
    std::string category_name;
    std::string extra;
    header_iss >> category_type >> category_name >> extra;
    if (category_type.empty() || category_name.empty() || !extra.empty()) {
        return std::unexpected{location_error(loc, "malformed @add_pak macro; expected '<category-type> <category-name>' before '{'")};
    }
    if (!pak68_type_from_string(category_type)) {
        return std::unexpected{location_error(loc, "unknown pak68 category type: " + category_type)};
    }

    std::string_view after_open = rest.substr(open_brace + 1);
    return add_pak_header{std::move(category_type), std::move(category_name), after_open};
}

[[nodiscard]] static std::optional<std::string> parse_add_pak_entries_from_segment(
    std::string_view segment,
    const bool closes_block,
    const source_location& loc,
    pak68_edit_request& request) noexcept {

    if (closes_block) {
        const u64 close_brace = segment.find('}');
        const std::string_view after_close = trim(segment.substr(close_brace + 1));
        if (!after_close.empty()) {
            return location_error(loc, "unexpected text after @add_pak closing brace");
        }
        segment = segment.substr(0, close_brace);
    }

    if (trim(segment).empty()) {
        return std::nullopt;
    }

    std::expected<pak68_entry, std::string> entry = parse_pak68_entry(segment, loc);
    if (!entry) {
        return entry.error();
    }
    request.m_entries.push_back(std::move(*entry));
    return std::nullopt;
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
            return location_error(loc, "@standalone has been removed; omit @target to compile standalone");
        }

        if (line == "@repackage") {
            return location_error(loc, "@repackage has been removed; providing @mod now implies repackage");
        }

        const u64 first_space = line.find_first_of(' ', 1);
        if (first_space == std::string::npos) {
            return location_error(loc, "malformed precompiler directive");
        }

        const std::string_view directive_name(line.data() + 1, first_space - 1);
        if (directive_name == "add_pak") {
            stripped_source += '\n';
            line_map.push_back(loc);

            std::expected<add_pak_header, std::string> header = parse_add_pak_header(line, loc);
            if (!header) {
                return header.error();
            }

            pak68_edit_request request;
            request.m_categoryType = std::move(header->m_categoryType);
            request.m_levelName = std::move(header->m_categoryName);
            request.m_location = loc;

            std::string_view segment = header->m_afterOpen;
            bool closes_block = segment.find('}') != std::string_view::npos;
            if (std::optional<std::string> err = parse_add_pak_entries_from_segment(segment, closes_block, loc, request)) {
                return err;
            }

            while (!closes_block) {
                std::string raw_block_line;
                if (!std::getline(iss, raw_block_line)) {
                    return location_error(loc, "unterminated @add_pak macro; expected '}'");
                }
                ++line_number;
                const std::string block_line = normalize_directive_line(std::move(raw_block_line));
                const source_location block_loc{current_path, line_number};
                stripped_source += '\n';
                line_map.push_back(block_loc);

                segment = block_line;
                closes_block = segment.find('}') != std::string_view::npos;
                if (std::optional<std::string> err = parse_add_pak_entries_from_segment(segment, closes_block, block_loc, request)) {
                    return err;
                }
            }

            if (request.m_entries.empty()) {
                return location_error(loc, "@add_pak requires at least one entry");
            }
            options.m_pak68Edits.push_back(std::move(request));
            continue;
        }

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
            return location_error(loc, "@standalone has been removed; omit @target to compile standalone");
        } else if (directive_name == "target") {
            options.m_target = path;
        } else if (directive_name == "mod") {
            options.m_mod = path;
        } else if (directive_name == "output") {
            options.m_output = path;
        } else if (directive_name == "sidbase") {
            options.m_sidbase = path;
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

    std::optional<std::filesystem::path> mod_filepath;
    if (args.count("mod") == 1) {
       mod_filepath = args["mod"].as<std::string>();
    }

    std::filesystem::path sidbase_filepath;
    if (args.count("s") == 1) {
       sidbase_filepath = args["s"].as<std::string>();
    }

    compiler_options out;
    out.m_target = std::move(target_binary_filepath);
    out.m_mod = std::move(mod_filepath);
    out.m_output = std::move(output_filepath);
    out.m_sidbase = std::move(sidbase_filepath);
    return out;
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

[[nodiscard]] std::expected<std::optional<std::filesystem::path>, std::string> check_optional_path_provided(
    const std::optional<std::filesystem::path>& lhs,
    const std::optional<std::filesystem::path>& rhs,
    std::string param_name,
    const bool check_directory = false) noexcept {

    if (lhs && rhs) {
        return std::unexpected{"provided both a command line argument and a precompiler directive for the option '" + std::move(param_name) + "\'. please only provide one or the other."};
    }

    std::optional<std::filesystem::path> out = lhs ? lhs : rhs;
    if (out) {
        if (!std::filesystem::exists(*out)) {
            return std::unexpected{"expected path to exist but got missing path " + out->string()};
        }
        if (check_directory && !std::filesystem::is_directory(*out)) {
            return std::unexpected{"expected directory but got non-directory path " + out->string()};
        }
    }
    return out;
}

[[nodiscard]] static std::filesystem::path append_bin_extension_if_missing(std::filesystem::path path) {
    if (!path.has_extension()) {
        path += ".bin";
    }
    return path;
}

[[nodiscard]] static bool path_component_equals(const std::filesystem::path& lhs, const char* rhs) {
    const std::string lhs_str = lhs.string();
    const std::string_view rhs_str{rhs};
    if (lhs_str.size() != rhs_str.size()) {
        return false;
    }
    for (u32 i = 0; i < lhs_str.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(lhs_str[i])) != std::tolower(static_cast<unsigned char>(rhs_str[i]))) {
            return false;
        }
    }
    return true;
}

[[nodiscard]] static bool path_starts_with_bin_dc1(const std::filesystem::path& path) {
    auto iter = path.begin();
    if (iter == path.end() || !path_component_equals(*iter, "bin")) {
        return false;
    }
    ++iter;
    return iter != path.end() && path_component_equals(*iter, "dc1");
}

[[nodiscard]] static std::filesystem::path resolve_mod_output_path(const std::filesystem::path& mod, std::filesystem::path output) {
    if (output.has_root_directory()) {
        output = output.relative_path();
    }
    if (path_starts_with_bin_dc1(output)) {
        return append_bin_extension_if_missing(mod / output);
    }
    return append_bin_extension_if_missing(mod / "bin" / "dc1" / output);
}

[[nodiscard]] static std::string mod_folder_name(const std::filesystem::path& mod) {
    if (!mod.filename().empty()) {
        return mod.filename().string();
    }
    return mod.parent_path().filename().string();
}

[[nodiscard]] static std::filesystem::path resolve_mod_pak68_path(const std::filesystem::path& mod) {
    return mod / (mod_folder_name(mod) + "-pak68.txt");
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

    std::expected<std::optional<std::filesystem::path>, std::string> mod_res =
        check_optional_path_provided(from_args->m_mod, from_dcpl->m_mod, "mod", true);
    if (!mod_res) {
        return std::unexpected{std::move(mod_res.error())};
    }
    std::optional<std::filesystem::path> mod = std::move(*mod_res);

    std::expected<std::filesystem::path, std::string> output_res = check_single_path_provided(from_args->m_output, from_dcpl->m_output, "output", false);
    if (!output_res) {
        return std::unexpected{std::move(output_res.error())};
    }

    std::filesystem::path target;
    if (!from_args->m_target.empty() && !from_dcpl->m_target.empty()) {
        return std::unexpected{"provided both a command line argument and a precompiler directive for the option 'target'. please only provide one or the other."};
    } else if (!from_args->m_target.empty()) {
        target = std::move(from_args->m_target);
    } else {
        target = std::move(from_dcpl->m_target);
    }
    if (!target.empty() && !std::filesystem::exists(target)) {
        return std::unexpected{"expected file to exist but got missing path " + target.string()};
    }
    const bool standalone = target.empty();

    const std::filesystem::path output_directive = *output_res;
    std::filesystem::path output = output_directive;
    if (mod) {
        if (output.is_absolute() || output.has_root_name()) {
            return std::unexpected{"@output must be relative when @mod is provided"};
        }
        output = resolve_mod_output_path(*mod, std::move(output));
    }

    std::filesystem::path modules;
    if (mod) {
        const std::filesystem::path derived_modules = *mod / "bin" / "dc1" / "modules.bin";
        if (!standalone || std::filesystem::exists(derived_modules)) {
            if (!std::filesystem::exists(derived_modules)) {
                return std::unexpected{"expected modules.bin at " + derived_modules.string()};
            }
            modules = derived_modules;
        }
    } else if (!standalone) {
        return std::unexpected{"non-standalone compile requires @mod so modules.bin can be resolved"};
    }

    std::expected<std::filesystem::path, std::string> sidbase_res = check_single_path_provided(from_args->m_sidbase, from_dcpl->m_sidbase, "sidbase");
    if (!sidbase_res) {
        return std::unexpected{std::move(sidbase_res.error())};
    }

    std::optional<std::filesystem::path> repackage_res = mod;

    std::optional<std::filesystem::path> pak68_res = std::nullopt;
    if (mod) {
        pak68_res = resolve_mod_pak68_path(*mod);
    } else if (!from_dcpl->m_pak68Edits.empty()) {
        return std::unexpected{"@add_pak requires @mod so the pak68 file can be resolved"};
    }

    if (pak68_res && !from_dcpl->m_pak68Edits.empty()) {
        if (const std::optional<std::string> pak68_err = validate_pak68_edits(*pak68_res, from_dcpl->m_pak68Edits)) {
            return std::unexpected{*pak68_err};
        }
    }

    compiler_options out;
    out.m_target = std::move(target);
    out.m_mod = std::move(mod);
    out.m_output = std::move(output);
    out.m_modules = std::move(modules);
    out.m_sidbase = std::move(*sidbase_res);
    out.m_repackage = std::move(repackage_res);
    out.m_pak68 = std::move(pak68_res);
    out.m_pak68Edits = std::move(from_dcpl->m_pak68Edits);
    out.m_standalone = standalone;
    return out;
}

[[nodiscard]] std::expected<compiler_options, std::string> compiler_options::parse(const cxxopts::ParseResult& args, std::string& source) noexcept {
    std::vector<source_location> line_map;
    return parse(args, source, {}, line_map);
}


}
