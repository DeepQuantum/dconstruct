#include "compilation/pak68.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace dconstruct::compilation {

namespace {

struct pak68_type_name {
    pak68_type m_type;
    std::string_view m_name;
};

constexpr pak68_type_name pak68_type_names[] = {
    {pak68_type::ACTOR, "actor"},
    {pak68_type::CACHE, "cache"},
    {pak68_type::IR_PACK, "ir-pack"},
    {pak68_type::LEVEL_ID, "level-id"},
    {pak68_type::LEVEL_NAME, "level-name"},
    {pak68_type::LUT_TABLE, "lut-table"},
    {pak68_type::PACKAGE, "package"},
    {pak68_type::PARTICLE_MODULE, "particle-module"},
    {pak68_type::RENDER_SETTINGS, "render-settings"},
    {pak68_type::SOUND_BANK, "sound-bank"},
    {pak68_type::SYMBOL, "symbol"},
    {pak68_type::VOX_CHARACTER, "vox-character"},
};

[[nodiscard]] std::string trim_cr(std::string line) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    return line;
}

[[nodiscard]] std::expected<std::vector<std::string>, std::string> load_lines(const std::filesystem::path& path) noexcept {
    std::ifstream in{path};
    if (!in.is_open()) {
        return std::unexpected{"couldn't open pak68 file " + path.string()};
    }

    std::vector<std::string> lines;
    for (std::string line; std::getline(in, line);) {
        lines.push_back(trim_cr(std::move(line)));
    }
    return lines;
}

[[nodiscard]] std::string first_token(const std::string& line) {
    std::istringstream iss{line};
    std::string token;
    iss >> token;
    return token;
}

[[nodiscard]] std::optional<std::string> level_name_from_line(const std::string& line) {
    std::istringstream iss{line};
    std::string type;
    std::string level_name;
    iss >> type >> level_name;
    if (type == "level-name" && !level_name.empty()) {
        return level_name;
    }
    return std::nullopt;
}

[[nodiscard]] std::optional<u64> find_level_start(const std::vector<std::string>& lines, const std::string& level_name) {
    for (u64 i = 0; i < lines.size(); ++i) {
        if (level_name_from_line(lines[i]) == level_name) {
            return i;
        }
    }
    return std::nullopt;
}

[[nodiscard]] u64 find_level_end(const std::vector<std::string>& lines, const u64 level_start) {
    for (u64 i = level_start + 1; i < lines.size(); ++i) {
        if (level_name_from_line(lines[i])) {
            return i;
        }
    }
    return lines.size();
}

[[nodiscard]] std::string entry_to_line(const pak68_entry& entry) {
    return std::string{pak68_type_to_string(entry.m_type)} + " " + entry.m_sid;
}

}

[[nodiscard]] std::optional<pak68_type> pak68_type_from_string(const std::string_view name) noexcept {
    for (const pak68_type_name& type_name : pak68_type_names) {
        if (type_name.m_name == name) {
            return type_name.m_type;
        }
    }
    return std::nullopt;
}

[[nodiscard]] std::string_view pak68_type_to_string(const pak68_type type) noexcept {
    for (const pak68_type_name& type_name : pak68_type_names) {
        if (type_name.m_type == type) {
            return type_name.m_name;
        }
    }
    return "<unknown>";
}

[[nodiscard]] std::optional<std::string> validate_pak68_edits(
    const std::filesystem::path& path,
    const std::vector<pak68_edit_request>& requests) noexcept {

    std::error_code ec;
    if (!std::filesystem::exists(path, ec)) {
        return "expected pak68 file to exist but got missing path " + path.string();
    }
    if (!std::filesystem::is_regular_file(path, ec)) {
        return "expected pak68 path to be a file but got " + path.string();
    }

    const std::expected<std::vector<std::string>, std::string> lines = load_lines(path);
    if (!lines) {
        return lines.error();
    }

    return std::nullopt;
}

[[nodiscard]] std::expected<std::vector<pak68_edit_summary>, std::string> apply_pak68_edits(
    const std::filesystem::path& path,
    const std::vector<pak68_edit_request>& requests) noexcept {

    if (requests.empty()) {
        return std::vector<pak68_edit_summary>{};
    }

    if (const std::optional<std::string> validation_err = validate_pak68_edits(path, requests)) {
        return std::unexpected{*validation_err};
    }

    std::expected<std::vector<std::string>, std::string> lines = load_lines(path);
    if (!lines) {
        return std::unexpected{lines.error()};
    }

    std::unordered_map<std::string, std::vector<pak68_entry>> entries_by_level;
    for (const pak68_edit_request& request : requests) {
        auto& entries = entries_by_level[request.m_levelName];
        entries.insert(entries.end(), request.m_entries.begin(), request.m_entries.end());
    }

    std::vector<pak68_edit_summary> summaries;
    for (const auto& [level_name, entries] : entries_by_level) {
        u64 level_start = 0;
        if (const std::optional<u64> existing_level_start = find_level_start(*lines, level_name)) {
            level_start = *existing_level_start;
        } else {
            lines->push_back("level-name " + level_name);
            level_start = lines->size() - 1;
        }

        const u64 level_end = find_level_end(*lines, level_start);
        std::unordered_set<std::string> existing_lines;
        for (u64 i = level_start + 1; i < level_end; ++i) {
            existing_lines.insert((*lines)[i]);
        }

        pak68_edit_summary summary{path, level_name};
        std::vector<std::string> lines_to_insert;
        for (const pak68_entry& entry : entries) {
            const std::string line = entry_to_line(entry);
            if (existing_lines.contains(line)) {
                summary.m_alreadyPresent.push_back(entry);
                continue;
            }
            existing_lines.insert(line);
            lines_to_insert.push_back(line);
            summary.m_added.push_back(entry);
        }

        lines->insert(lines->begin() + static_cast<std::ptrdiff_t>(level_end), lines_to_insert.begin(), lines_to_insert.end());
        summaries.push_back(std::move(summary));
    }

    std::ofstream out{path, std::ios::trunc};
    if (!out.is_open()) {
        return std::unexpected{"couldn't write pak68 file " + path.string()};
    }
    for (const std::string& line : *lines) {
        out << line << '\n';
    }

    return summaries;
}

}
