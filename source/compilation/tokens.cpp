#include "compilation/tokens.h"
#include <format>


namespace dconstruct::compilation {

    const std::filesystem::path* source_file_table::intern(std::filesystem::path path) {
        const std::string key = path.string();
        if (const auto it = m_filesByPath.find(key); it != m_filesByPath.end()) {
            return it->second;
        }

        std::unique_ptr<std::filesystem::path> stored = std::make_unique<std::filesystem::path>(std::move(path));
        const std::filesystem::path* stored_ptr = stored.get();
        m_files.push_back(std::move(stored));
        m_filesByPath.emplace(key, stored_ptr);
        return stored_ptr;
    }

    void source_file_table::clear() noexcept {
        m_filesByPath.clear();
        m_files.clear();
    }

    [[nodiscard]] sid64 get_identifier_sid(const std::string& identifier) {
        if (identifier.size() == 17 && identifier[0] == '#') {
            for (u32 i = 1; i < identifier.size(); ++i) {
                const char c = identifier[i];
                const bool is_num = c >= '0' && c <= '9';
                const bool is_upper_hex = c >= 'A' && c <= 'F';
                if (!is_num && !is_upper_hex) {
                    return SID(identifier.c_str() + 1);
                }
            }
            return std::stoull(identifier.substr(1), nullptr, 16);
        }
        return SID(identifier.c_str() + 1);
    }

    [[nodiscard]] std::string format_source_location(const source_location& location) {
        return std::format("file: {}: line {}, column: {}", location.m_file ? location.m_file->string() : "<anonymous>", location.m_line, location.m_char);
    }

    [[nodiscard]] bool token::operator==(const token& rhs) const {
        return m_type == rhs.m_type && m_lexeme == rhs.m_lexeme && m_literal == rhs.m_literal && m_line == rhs.m_line && m_file == rhs.m_file;
    }

}
