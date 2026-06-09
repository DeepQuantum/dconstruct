#include "debugger/debugger.h"

#include <array>
#include <cassert>
#include <chrono>
#include <optional>
#include <print>
#include <thread>

using namespace std::chrono_literals;

int main() {
    using namespace dconstruct::debugger;

    constexpr auto sids = std::array{SID("ss-main-menu-lightbar-controller player-ellie (on ((update)))")};
    debugger db{};
    db.set_sids(sids.begin(), sids.begin() + sids.size());

    constexpr u32 steps_to_take = 3;
    u32 steps_requested = 0;
    bool attach_requested = false;

    while (true) {
        switch (db.poll_state()) {
            case debugger::STATE::DETACHED: {
                if (attach_requested) {
                    return 0;
                }
                if (std::optional<std::string> attach_error = db.request_attach()) {
                    std::println(stderr, "error: {}", *attach_error);
                    return 1;
                }
                attach_requested = true;
                break;
            }
            case debugger::STATE::ATTACHING: std::println(stderr, "waiting for attach..."); break;
            case debugger::STATE::ATTACHED: {
                std::println(stderr, "attached, but no bp hit yet.");
                break;
            }
            case debugger::STATE::SNAPSHOT_READY: {
                std::shared_ptr res = db.poll_snapshot();
                assert(res);
                std::println(stderr, "got function: {}", res->m_func->get_id());
                if (steps_requested < steps_to_take) {
                    ++steps_requested;
                    std::println(stderr, "step instruction {}/{}", steps_requested, steps_to_take);
                    db.request_single_step();
                    break;
                }

                db.detach();
                std::println(stderr, "detached after {} instruction steps.", steps_to_take);
                return 0;
            }
            case debugger::STATE::ERROR: {
                if (std::optional<std::string> error = db.poll_error()) {
                    std::println(stderr, "error: {}", *error);
                } else {
                    std::println(stderr, "error");
                }
                db.detach();
                return 1;
            }
        }

        std::this_thread::sleep_for(100ms);
    }
}
