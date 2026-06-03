#include "base.h"

#include "ctre.hpp"

namespace dconstruct {

    using namespace std::literals;

    static constexpr std::array BOXED_VALUE_IDS = {
        "invalid"sv,
        "bool"sv,
        "i32"sv,
        "u32"sv,
        "i64"sv,
        "u64"sv,
        "f32"sv,
        "symbol"sv,
        "timeframePOD"sv,
        "unused"sv,
        "netTrackerId"sv,
        "processHandleMutable"sv,
        "processHandle"sv,
        "p64"sv,
        "point"sv,
        "vector"sv,
        "quat"sv,
        "locator"sv,
        "boundframe"sv,
        "stringcopy"sv,
        "unused"sv,
        "array"sv,
        "color"sv,
        "ssAnimateParams"sv,
        "NDattackInfo"sv,
    };

    [[nodiscard]] std::string apply_decomp_regex_transformations(std::string&& input);

}
