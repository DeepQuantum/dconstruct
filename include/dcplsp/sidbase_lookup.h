#pragma once


#include "lsp/types.h"
#include "sidbase.h"

namespace dconstruct::dcplsp {
    static constexpr u64 MAX_SIDBASE_MATCHES = 100;

    [[nodiscard]] lsp::CompletionList get_sidbase_matches(const lsp::String& needle, const dconstruct::SIDBase& sidbase, const u64 max_sidbase_matches = MAX_SIDBASE_MATCHES);
}
