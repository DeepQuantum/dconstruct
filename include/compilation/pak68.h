#pragma once

#include "compilation/tokens.h"

#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace dconstruct::compilation {

    enum class pak68_type {
        ACTOR,
        CACHE,
        IR_PACK,
        LEVEL_ID,
        LEVEL_NAME,
        LUT_TABLE,
        PACKAGE,
        PARTICLE_MODULE,
        RENDER_SETTINGS,
        SOUND_BANK,
        SYMBOL,
        VOX_CHARACTER,
    };

    struct pak68_entry {
        pak68_type m_type;
        std::string m_sid;

        [[nodiscard]] bool operator==(const pak68_entry&) const noexcept = default;
    };

    struct pak68_edit_request {
        std::string m_levelName;
        std::vector<pak68_entry> m_entries;
        source_location m_location;
    };

    struct pak68_edit_summary {
        std::filesystem::path m_path;
        std::string m_levelName;
        std::vector<pak68_entry> m_added;
        std::vector<pak68_entry> m_alreadyPresent;
    };

    [[nodiscard]] std::optional<pak68_type> pak68_type_from_string(std::string_view name) noexcept;
    [[nodiscard]] std::string_view pak68_type_to_string(pak68_type type) noexcept;

    [[nodiscard]] std::optional<std::string> validate_pak68_edits(
        const std::filesystem::path& path,
        const std::vector<pak68_edit_request>& requests) noexcept;

    [[nodiscard]] std::expected<std::vector<pak68_edit_summary>, std::string> apply_pak68_edits(
        const std::filesystem::path& path,
        const std::vector<pak68_edit_request>& requests) noexcept;

}
