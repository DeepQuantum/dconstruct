#ifndef BASE_H
#define BASE_H

#include <cstdint>

typedef uint64_t stringid_64;


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

#define SID(str) (ToStringId64(str))

constexpr stringid_64 ToStringId64(const char* str) {
	u64 base = 0xCBF29CE484222325;
	if (*str) {
		do {
			base = 0x100000001B3 * (base ^ *str++);
		} while (*str);
	}
	return base;
}

#endif // BASE_H
