#include "debugger/debugger.h"

#include <array>
#include <optional>
#include <print>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

int main() {
    using namespace dconstruct::debugger;

    constexpr auto sids = std::array{SID("ss-main-menu-lightbar-controller player-ellie (on ((update)))")};
    debugger db{};
    db.set_sids(sids.begin(), sids.begin() + sids.size());


    while (true) {
        switch (db.poll_state()) {
            case debugger::STATE::DETACHED: {
                if (std::optional<std::string> attach_error = db.request_attach()) {
                    std::println(stderr, "error: {}", *attach_error);
                    return 1;
                }
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
                break;
            }
            case debugger::STATE::ERROR: {
                std::println(stderr, "error: {}", *db.poll_error());
                db.detach();
                break;
            }
        }

        std::this_thread::sleep_for(1s);
    }


    return 0;
}
