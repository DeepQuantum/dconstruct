#include "compilation/global_state.h"
#include <sstream>
#include <iomanip>

namespace dconstruct::compiler { 

[[nodiscard]] u64 global_state::add_string_as_placeholder(std::string str) noexcept {
    const u32 current_size = m_strings.size();
    m_strings.push_back(std::move(str));

    std::string placeholder = "STRPL";
    for (std::size_t i = 0; i < 3; ++i) {
        placeholder.push_back(static_cast<char>((current_size >> (i * 8)) & 0xFF));
    }

    assert(placeholder.size() == 8);

    const char *out = placeholder.c_str();
    const u64 hex_value = *reinterpret_cast<const u64*>(out);

    return hex_value;
}

[[nodiscard]] u64 global_state::add_string(std::string str) noexcept {
    const u64 size = m_strings.size();
    m_strings.push_back(std::move(str));
    return size;
}
}