#pragma once

#include "base.h"
#include "disassembly/instructions.h"


namespace dconstruct::compiler {
    struct global_state {
        std::vector<std::string> m_strings;

        [[nodiscard]] u64 add_string_as_placeholder(std::string str) noexcept;
        [[nodiscard]] u64 add_string(std::string str) noexcept;
    };
}