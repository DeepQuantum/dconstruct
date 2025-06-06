#pragma once 
#include "base.h"

namespace dc_structs {
    struct array {
        const u64* data;
        const u64 &operator[](const u64 idx) const noexcept {
            return this->data[idx];
        }
    };

    struct symbol_array_contents {
        u64 size;
        array keys;
    };

    struct symbol_array {
        symbol_array_contents contents;
    };

    struct map {
        u32 size;
        array keys;
        array values;
    };
}
