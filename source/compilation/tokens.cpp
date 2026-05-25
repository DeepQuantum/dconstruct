#include "compilation/tokens.h"

namespace dconstruct::compilation {


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
    if (location.m_file.empty()) {
        return "line " + std::to_string(location.m_line);
    }
    return location.m_file.string() + ":" + std::to_string(location.m_line);
}


[[nodiscard]] bool token::operator==(const token &rhs) const {
    return m_type == rhs.m_type && m_lexeme == rhs.m_lexeme && m_literal == rhs.m_literal && m_line == rhs.m_line && m_file == rhs.m_file;
}

}
