#include "debugger/debugger.h"

#include <array>
#include <expected>
#include <optional>
#include <print>

int main() {
    constexpr auto sids = std::array{SID("ss-main-menu-lightbar-controller player-ellie (on ((update)))")};
    dconstruct::debugger::debugger db{};
    if (std::optional<std::string> attach_error = db.attach()) {
        std::println(stderr, "{}", *attach_error);
        return 1;
    }
    std::expected function_disassembly = db.get_sid_matched_function_disassembly(sids.data(), sids.data() + sids.size());
    if (!function_disassembly) {
        std::println(stderr, "{}", function_disassembly.error());
        return 1;
    }
    return 0;
}
