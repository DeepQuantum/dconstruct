#pragma once

#include "base.h"
#include "disassembly/instructions.h"
#include <unordered_map>

namespace dconstruct::compiler {
    struct global_state {
        std::vector<std::string> m_strings;
        std::unordered_map<std::string, std::pair<ast::full_type, sid64>> m_sidAliases;

        [[nodiscard]] u64 add_string_as_placeholder(std::string str) noexcept;
        [[nodiscard]] u64 add_string(std::string str) noexcept;
    };
}