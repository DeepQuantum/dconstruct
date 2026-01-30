#include "compilation/preprocessor.h"

#include <sstream>
#include <string_view>

namespace dconstruct::compilation {

[[nodiscard]] std::expected<required_options, std::string> required_options::from_args(const cxxopts::ParseResult& args) noexcept {
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

    return required_options{std::move(target_binary_filepath), std::move(output_filepath), std::move(modules_filepath), std::move(sidbase_filepath)}; 
}

[[nodiscard]] std::expected<required_options, std::string> required_options::from_dcpl(std::string& source) noexcept {
    std::istringstream iss(source);

    required_options out;
    std::string stripped_source;
    stripped_source.reserve(source.size());

    for (std::string line; std::getline(iss, line);) {
        if (!line.starts_with("@")) {
            stripped_source += line;
            stripped_source += '\n';
            continue;
        }

        const u64 first_space = line.find_first_of(' ', 1);
        if (first_space == std::string::npos) {
            return std::unexpected{"malformed precompiler directive"};
        }

        const std::string_view directive_name(line.data() + 1, first_space - 1);
        const std::string_view path(line.data() + first_space + 2, line.size() - first_space - 3);

        if (directive_name == "target") {
            out.m_target = path;
        } else if (directive_name == "output") {
            out.m_output = path;
        } else if (directive_name == "modules") {
            out.m_modules = path;
        } else if (directive_name == "sidbase") {
            out.m_sidbase = path;
        } else {
            return std::unexpected{
                "unknown precompiler directive: " + std::string(directive_name)
            };
        }
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
            return std::unexpected{"couldn't find file " + lhs.string()};
        } else {
            return lhs;
        }
    } else {
        if (!std::filesystem::exists(rhs) && check_exists) {
            return std::unexpected{"couldn't find file " + rhs.string()};
        } else {
            return rhs;
        }
    }
}

[[nodiscard]] std::expected<required_options, std::string> required_options::parse(const cxxopts::ParseResult& args, std::string& source) noexcept {
    std::expected<required_options, std::string> from_args = required_options::from_args(args);
    if (!from_args) {
        return from_args;
    }
    std::expected<required_options, std::string> from_dcpl = required_options::from_dcpl(source);
    if (!from_dcpl) {
        return from_dcpl;
    }
    
    std::expected<std::filesystem::path, std::string> output_res = check_single_path_provided(from_args->m_output, from_dcpl->m_output, "output", false);
    if (!output_res) {
        return std::unexpected{std::move(output_res.error())};
    }

    std::expected<std::filesystem::path, std::string> target_res = check_single_path_provided(from_args->m_target, from_dcpl->m_target, "target");
    if (!target_res) {
        return std::unexpected{std::move(target_res.error())};
    }

    std::expected<std::filesystem::path, std::string> modules_res = check_single_path_provided(from_args->m_modules, from_dcpl->m_modules, "modules");
    if (!modules_res) {
        return std::unexpected{std::move(modules_res.error())};
    }

    std::expected<std::filesystem::path, std::string> sidbase_res = check_single_path_provided(from_args->m_sidbase, from_dcpl->m_sidbase, "sidbase");
    if (!sidbase_res) {
        return std::unexpected{std::move(sidbase_res.error())};
    }

    return required_options{std::move(*output_res), std::move(*target_res), std::move(*modules_res), std::move(*sidbase_res)};
}


}