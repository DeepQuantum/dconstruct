#pragma once 
#include "base.h"
#include "DCScript.h"

namespace dc_structs {
    struct unmapped {
        const sid64 typeID;
        const void *m_data;
    };
    
    struct array {
        const u64* data;
        const u64 &operator[](const u64 idx) const noexcept {
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

    // struct level_entry {
    //     const char* text;
    //     sid64 sid;
    //     u32 u32_1;
    //     u32 u32_2;
    // };

    // struct level_set_arrays {
    //     const struct {
    //         const level_entry **ptr;
    //         const u32 size;
    //         const i32 int0;
    //     };
    // };

    // struct level_set {
    //     const char* set_id;
    //     const sid64 set_id_sid;
    //     const struct {
    //         level_set_arrays lsa;
    //         const struct {
    //             const struct {
    //                 const sid64 sid1;
    //                 const i32 int1;
    //                 const i32 int2;
    //                 const i32 int3;
    //                 const i32 int4;
    //             } **ptr;
    //             const i32 int1;
    //             const i32 int2;
    //         } *level_entries;
    //     } *level_data;
    //     const struct {
    //         const ScriptLambda *lambda;
    //         const level_set_arrays *lsa;
    //     } **level_scripted_set;
    //     const i32 int0;
    //     const i32 int1;
    //     const i32 int2;
    //     const i32 int3;
    // };

    struct point_curve {
        const u32 int1;
        const f32 floats[33];
    };

    // struct string_array {
    //     const u64 size;
    //     const array contents;
    // };
}
