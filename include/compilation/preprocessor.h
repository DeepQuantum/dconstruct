#pragma once

#include "base.h"

#include <filesystem>
#include <expected>
#include "cxxopts.hpp"

namespace dconstruct::compilation {
    struct required_options {
        std::filesystem::path m_target;
        std::filesystem::path m_output;
        std::filesystem::path m_modules;
        std::filesystem::path m_sidbase;

        
        [[nodiscard]] static std::expected<required_options, std::string> parse(const cxxopts::ParseResult& args, std::string& source) noexcept;
    private:
        [[nodiscard]] static std::expected<required_options, std::string> from_args(const cxxopts::ParseResult& args) noexcept;
        [[nodiscard]] static std::expected<required_options, std::string> from_dcpl(std::string& source) noexcept;
    };

}