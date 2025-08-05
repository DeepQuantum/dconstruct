#pragma once 
#include "base.h"
#include "DCScript.h"

namespace dconstruct::structs {
    struct unmapped {
        const sid64 typeID;
        const void* m_data;
    };

    struct array {
        const u64* data;
        const u64& operator[](const u64 idx) const noexcept {
            return data[idx];
        }
    };

    struct symbol_array {
        const struct {
            u64 size;
            array keys;
        } contents;
    };

    struct map {
        u32 size;
        array keys;
        array values;
    };

    struct point_curve {
        const u32 int1;
        const f32 floats[33];
    };
}