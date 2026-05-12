#pragma once

#include "base.h"
#include "compilation/tokens.h"

#include <filesystem>
#include <expected>
#include <vector>
#include "cxxopts.hpp"

namespace dconstruct::compilation {
    struct compiler_options {
        std::filesystem::path m_target;
        std::filesystem::path m_output;
        std::filesystem::path m_modules;
        std::filesystem::path m_sidbase;
        std::optional<std::filesystem::path> m_repackage;
        bool m_standalone = false;

        
        [[nodiscard]] static std::expected<compiler_options, std::string> parse(
            const cxxopts::ParseResult& args,
            std::string& source,
            const std::filesystem::path& source_path,
            std::vector<source_location>& line_map) noexcept;
        [[nodiscard]] static std::expected<compiler_options, std::string> parse(const cxxopts::ParseResult& args, std::string& source) noexcept;
    private:
        [[nodiscard]] static std::expected<compiler_options, std::string> from_args(const cxxopts::ParseResult& args) noexcept;
        [[nodiscard]] static std::expected<compiler_options, std::string> from_dcpl(
            std::string& source,
            const std::filesystem::path& source_path,
            std::vector<source_location>& line_map) noexcept;
    };

}
