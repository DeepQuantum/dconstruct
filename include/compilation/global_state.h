#pragma once

#include "base.h"
#include "disassembly/instructions.h"
#include <unordered_map>

namespace dconstruct::compilation {
    struct global_state {
        [[nodiscard]] u64 add_string_as_placeholder(std::string str) noexcept;
        [[nodiscard]] u64 add_string(std::string str) noexcept;

        std::vector<std::string> m_strings;
    };
}