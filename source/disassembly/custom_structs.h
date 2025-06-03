#pragma once 
#include "base.h"


struct DCArray {
    const u64* data;
};

struct DCMap {
    u32 size;
    DCArray keys;
    DCArray values;
};