#pragma once 
#include "base.h"

namespace dc_structs {
    struct unmapped {
        sid64 m_typeID;
        const void *m_data;
    };
    
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

    struct level_set_array_entry {
        const u64 **arrays;
        u32 value1;
        i32 value2;
    };

    struct level_set_arrays {
        const level_set_array_entry entries[7];
    };

    struct point_curve {
        const u32 int1;
        const f32 floats[33];

        //constexpr static char *fmt = "";
    };
}
