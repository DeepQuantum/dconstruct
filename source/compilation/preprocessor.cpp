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

    [[nodiscard]] static resstr<std::filesystem::path> parse_quoted_path(
        const std::string& line,
        const u64 first_space,
        const source_location& loc
    ) noexcept {
        const u64 first_quote = line.find('"', first_space + 1);
        const u64 last_quote = line.rfind('"');
        if (first_quote == std::string::npos || last_quote == std::string::npos || first_quote == last_quote || last_quote != line.size() - 1) {
            return std::unexpected{location_error(loc, "malformed precompiler directive path")};
        }
        return std::filesystem::path{std::string(line.data() + first_quote + 1, last_quote - first_quote - 1)};
    }

    [[nodiscard]] static resstr<pak68_entry> parse_pak68_entry(
        const std::string_view line,
        const source_location& loc
    ) noexcept {
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

    [[nodiscard]] static resstr<add_pak_header> parse_add_pak_header(
        const std::string& line,
        const source_location& loc
    ) noexcept {
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

    [[nodiscard]] static errmsg parse_add_pak_entries_from_segment(
        std::string_view segment,
        const bool closes_block,
        const source_location& loc,
        pak68_edit_request& request
    ) noexcept {
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

        resstr<pak68_entry> entry = parse_pak68_entry(segment, loc);
        if (!entry) {
            return entry.error();
        }
        request.m_entries.push_back(std::move(*entry));
        return std::nullopt;
    }

    [[nodiscard]] static errmsg preprocess_dcpl(
        const std::string& source,
        const std::filesystem::path& source_path,
        compiler_options& options,
        std::unordered_set<std::string>& included_paths,
        std::string& stripped_source,
        std::vector<source_location>& line_map
    ) noexcept {
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

            const u64 first_space = line.find_first_of(' ', 1);
            if (first_space == std::string::npos) {
                return location_error(loc, "malformed precompiler directive");
            }

            const std::string_view directive_name(line.data() + 1, first_space - 1);
            if (directive_name == "add_pak") {
                stripped_source += '\n';
                line_map.push_back(loc);

                resstr<add_pak_header> header = parse_add_pak_header(line, loc);
                if (!header) {
                    return header.error();
                }

                pak68_edit_request request;
                request.m_categoryType = std::move(header->m_categoryType);
                request.m_levelName = std::move(header->m_categoryName);
                request.m_location = loc;

                std::string_view segment = header->m_afterOpen;
                bool closes_block = segment.find('}') != std::string_view::npos;
                if (errmsg err = parse_add_pak_entries_from_segment(segment, closes_block, loc, request)) {
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
                    if (errmsg err = parse_add_pak_entries_from_segment(segment, closes_block, block_loc, request)) {
                        return err;
                    }
                }

                if (request.m_entries.empty()) {
                    return location_error(loc, "@add_pak requires at least one entry");
                }
                options.m_pak68Edits.push_back(std::move(request));
                continue;
            }

            resstr<std::filesystem::path> path_res = parse_quoted_path(line, first_space, loc);
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
                if (errmsg err = preprocess_dcpl(input.str(), include_path, options, included_paths, stripped_source, line_map)) {
                    return err;
                }
                continue;
            }

            stripped_source += '\n';
            line_map.push_back(loc);

            if (directive_name == "game") {
                options.m_game = path;
            } else if (directive_name == "target") {
                if (!options.m_target.empty()) {
                    return location_error(loc, "multiple @target directives are not allowed");
                }
                options.m_target = path;
            } else if (directive_name == "mod") {
                options.m_mod = path;
            } else {
                return location_error(loc, "unknown precompiler directive: " + std::string(directive_name));
            }
        }

        return std::nullopt;
    }

    [[nodiscard]] std::filesystem::path game_dc1_dir(const std::filesystem::path& game) noexcept {
        return game / "build" / "pc" / "main" / "bin_unpacked" / "dc1";
    }

    [[nodiscard]] resstr<compiler_options> compiler_options::from_dcpl(
        std::string& source,
        const std::filesystem::path& source_path,
        std::vector<source_location>& line_map
    ) noexcept {
        compiler_options out;
        std::string stripped_source;
        stripped_source.reserve(source.size());
        line_map.clear();

        std::unordered_set<std::string> included_paths;
        included_paths.insert(canonical_existing_path(source_path).string());
        if (errmsg err = preprocess_dcpl(source, source_path, out, included_paths, stripped_source, line_map)) {
            return std::unexpected{std::move(*err)};
        }
        stripped_source.shrink_to_fit();

        source = std::move(stripped_source);
        return out;
    }

    [[nodiscard]] static std::filesystem::path append_bin_extension_if_missing(std::filesystem::path path) {
        if (!path.has_extension()) {
            path += ".bin";
        }
        return path;
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

    [[nodiscard]] resstr<compiler_options> compiler_options::parse(
        const cxxopts::ParseResult& args,
        std::string& source,
        const std::filesystem::path& source_path,
        std::vector<source_location>& line_map
    ) noexcept {
        resstr<compiler_options> from_dcpl = compiler_options::from_dcpl(source, source_path, line_map);
        if (!from_dcpl) {
            return from_dcpl;
        }

        const std::filesystem::path game = from_dcpl->m_game;
        if (game.empty()) {
            return std::unexpected{"missing required @game directive"};
        }
        if (!std::filesystem::is_directory(game)) {
            return std::unexpected{"@game must point to an existing game directory but got " + game.string()};
        }

        if (!from_dcpl->m_mod || from_dcpl->m_mod->empty()) {
            return std::unexpected{"missing required @mod directive"};
        }
        const std::filesystem::path mod_name = *from_dcpl->m_mod;
        if (mod_name.is_absolute() || mod_name.has_root_name()) {
            return std::unexpected{"@mod must be a relative mod name placed under <game>/mods but got " + mod_name.string()};
        }

        const std::filesystem::path game_dc1 = game_dc1_dir(game);
        const std::filesystem::path mod_dir = game / "mods" / mod_name;
        const std::filesystem::path mod_dc1 = mod_dir / "bin" / "dc1";

        const std::filesystem::path& target_relative = from_dcpl->m_target;
        const bool standalone = target_relative.empty();

        std::filesystem::path target;
        std::filesystem::path output;
        if (standalone) {
            output = mod_dc1 / (mod_folder_name(mod_dir) + ".bin");
        } else {
            if (target_relative.is_absolute() || target_relative.has_root_name()) {
                return std::unexpected{"@target must be relative to the game's dc1 directory but got " + target_relative.string()};
            }
            target = append_bin_extension_if_missing(game_dc1 / target_relative);
            if (!std::filesystem::exists(target)) {
                return std::unexpected{"expected target file to exist in the game's dc1 directory but got missing path " + target.string()};
            }
            output = append_bin_extension_if_missing(mod_dc1 / target_relative);
        }

        const std::filesystem::path modules = mod_dc1 / "modules.bin";

        std::filesystem::path sidbase = executable_relative_sidbase();
        if (!std::filesystem::exists(sidbase)) {
            return std::unexpected{"expected sidbase.bin next to the executable but got missing path " + sidbase.string()};
        }

        const std::filesystem::path pak68 = resolve_mod_pak68_path(mod_dir);
        if (!from_dcpl->m_pak68Edits.empty()) {
            if (const errmsg pak68_err = validate_pak68_edits(pak68, from_dcpl->m_pak68Edits)) {
                return std::unexpected{*pak68_err};
            }
        }

        compiler_options out;
        out.m_game = game;
        out.m_target = std::move(target);
        out.m_mod = mod_dir;
        out.m_output = std::move(output);
        out.m_modules = modules;
        out.m_sidbase = std::move(sidbase);
        out.m_repackage = mod_dir;
        out.m_pak68 = pak68;
        out.m_pak68Edits = std::move(from_dcpl->m_pak68Edits);
        out.m_standalone = standalone;
        return out;
    }

    [[nodiscard]] resstr<compiler_options> compiler_options::parse(
        const cxxopts::ParseResult& args,
        std::string& source
    ) noexcept {
        std::vector<source_location> line_map;
        return parse(args, source, {}, line_map);
    }

}
