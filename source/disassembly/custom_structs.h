#pragma once 
#include "base.h"


struct DCArray {
    const u64* data;

    const u64 &operator[](const u64 idx) {
        return this->data[idx];
    }
};

struct DCMap {
    u32 size;
    DCArray keys;
    DCArray values;
};