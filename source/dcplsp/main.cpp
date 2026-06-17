#include "base.h"

#include "sidbase.h"
#include "dcplsp/server.h"
#include <expected>
#include <print>

using namespace std::string_view_literals;

i32 main () {

    auto exec_path = dconstruct::executable_path();

    auto sidbase_path = exec_path.parent_path() / "sidbase.bin";

    auto sidbase_res = dconstruct::SIDBase::from_binary(sidbase_path);

    if (!sidbase_res) {
        std::println(stderr, "{}", std::move(sidbase_res.error()));
        return -1;
    }

    sidbase_res->acquire_strings();

    dconstruct::dcplsp::server server(std::move(*sidbase_res));

    server.run();

    return 0;
}
