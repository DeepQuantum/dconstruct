#pragma once

#include "binaryfile.h"
#include "compilation/global_state.h"
#include "compilation/program_binary_element.h"
#include <expected>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace dconstruct::compilation {

    [[nodiscard]] resstr<std::pair<std::unique_ptr<std::byte[]>, u64>> patch_target_binary(
        const BinaryFile& file,
        std::vector<program_binary_element> elements,
        const global_state& global
    ) noexcept;

}
