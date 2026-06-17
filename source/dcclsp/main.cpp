#include "base.h"

#include "sidbase.h"
#include "lsp/server.h"
#include <expected>

using namespace std::string_view_literals;


i32 main () {

    auto sidbase_res = dconstruct::SIDBase::from_binary("./sidbase.bin");
    if (!sidbase_res) {
        exit(-1);
    }

    dconstruct::dcclsp::server server(std::move(*sidbase_res));     

    server.run();

    return 0;
}
