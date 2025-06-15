#pragma once

#include <string>
#include "stdio.h"
#include <cstdint>
#include <bit>

typedef uint64_t sid64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef bool b8;
typedef float f32;
typedef double f64;
typedef uintptr_t p64;

#define SID(str) (ToStringId64(str))

constexpr sid64 ToStringId64(const char* str) {
	u64 base = 0xCBF29CE484222325;
	if (*str) {
		do {
			base = 0x100000001B3 * (base ^ *str++);
		} while (*str);
	}
	return base;
}

inline const std::string int_to_string_id(sid64 sid) {
	char buffer[20] = {0};
    snprintf(buffer, sizeof(buffer), "#%016llX", sid);
    return buffer;
}

inline const std::string offset_to_string(u32 offset) {
    char buffer[20] = {0};
    snprintf(buffer, sizeof(buffer), "0x%06X", offset);
    return buffer;
}
