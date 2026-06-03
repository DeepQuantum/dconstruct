#include "compilation/pak68.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace dconstruct::compilation {

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
        std::error_code ec;
        if (!std::filesystem::exists(path, ec)) {
            return std::vector<std::string>{};
        }

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

    [[nodiscard]] std::string category_header_type(const std::string& category_type) {
        return category_type;
    }

    [[nodiscard]] std::optional<std::string> category_name_from_line(
        const std::string& line,
        const std::string& category_type
    ) {
        std::istringstream iss{line};
        std::string type;
        std::string category_name;
        iss >> type >> category_name;
        if (type == category_header_type(category_type) && !category_name.empty()) {
            return category_name;
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<u64> find_category_start(
        const std::vector<std::string>& lines,
        const std::string& category_type,
        const std::string& category_name
    ) {
        for (u64 i = 0; i < lines.size(); ++i) {
            if (category_name_from_line(lines[i], category_type) == category_name) {
                return i;
            }
        }
        return std::nullopt;
    }

    [[nodiscard]] u64 find_category_end(
        const std::vector<std::string>& lines,
        const std::string& category_type,
        const u64 category_start
    ) {
        for (u64 i = category_start + 1; i < lines.size(); ++i) {
            if (category_name_from_line(lines[i], category_type)) {
                return i;
            }
        }
        return lines.size();
    }

    [[nodiscard]] std::string entry_to_line(const pak68_entry& entry) {
        return std::string{pak68_type_to_string(entry.m_type)} + " " + entry.m_sid;
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
        const std::vector<pak68_edit_request>& requests
    ) noexcept {
        std::error_code ec;
        if (std::filesystem::exists(path, ec) && !std::filesystem::is_regular_file(path, ec)) {
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
        const std::vector<pak68_edit_request>& requests
    ) noexcept {
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

        struct category_key {
            std::string m_type;
            std::string m_name;

            [[nodiscard]] bool operator==(const category_key&) const noexcept = default;
        };

        struct category_key_hash {
            [[nodiscard]] u64 operator()(const category_key& key) const noexcept {
                const u64 type_hash = std::hash<std::string>{}(key.m_type);
                const u64 name_hash = std::hash<std::string>{}(key.m_name);
                return type_hash ^ (name_hash + 0x9e3779b97f4a7c15ULL + (type_hash << 6) + (type_hash >> 2));
            }
        };

        std::unordered_map<category_key, std::vector<pak68_entry>, category_key_hash> entries_by_category;
        for (const pak68_edit_request& request : requests) {
            auto& entries = entries_by_category[category_key{category_header_type(request.m_categoryType), request.m_levelName}];
            entries.insert(entries.end(), request.m_entries.begin(), request.m_entries.end());
        }

        std::vector<pak68_edit_summary> summaries;
        for (const auto& [category, entries] : entries_by_category) {
            u64 level_start = 0;
            if (const std::optional<u64> existing_level_start = find_category_start(*lines, category.m_type, category.m_name)) {
                level_start = *existing_level_start;
            } else {
                lines->push_back(category_header_type(category.m_type) + " " + category.m_name);
                level_start = lines->size() - 1;
            }

            const u64 level_end = find_category_end(*lines, category.m_type, level_start);
            std::unordered_set<std::string> existing_lines;
            for (u64 i = level_start + 1; i < level_end; ++i) {
                existing_lines.insert((*lines)[i]);
            }

            pak68_edit_summary summary{path, category.m_name};
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

        std::error_code ec;
        if (path.has_parent_path()) {
            std::filesystem::create_directories(path.parent_path(), ec);
            if (ec) {
                return std::unexpected{"couldn't create pak68 directory " + path.parent_path().string()};
            }
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
