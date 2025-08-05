#pragma once

#include <string>
#include <cstdio>
#include <cstdint>
#include <memory>


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

#define SID(str) (dconstruct::ToStringId64(str))

namespace dconstruct {
	struct location {
		const std::byte* m_ptr = nullptr;

		location() noexcept {};
		location(const void* ptr) noexcept : m_ptr(reinterpret_cast<const std::byte*>(ptr)) {};

		[[nodiscard]] location &from(const location& rhs, const i32 offset = 0) noexcept {
			m_ptr = rhs.get<std::byte*>() + offset;
			return *this;
		}

		template<typename T>
		[[nodiscard]] const T* as(const i32 offset = 0) const noexcept {
			return reinterpret_cast<const T*>(m_ptr + offset);
		}

		template<typename T>
		[[nodiscard]] const T& get(const i32 offset = 0) const noexcept {
			return *reinterpret_cast<const T*>(m_ptr + offset);
		}

		[[nodiscard]] p64 num() const noexcept {
			return reinterpret_cast<p64>(m_ptr);
		}

		[[nodiscard]] location aligned() const noexcept {
			return location(m_ptr - num() % 8);
		}

		[[nodiscard]] location operator+(const u64 rhs) const noexcept {
			return location(m_ptr + rhs);
		}

		[[nodiscard]] location operator-(const u64 rhs) const noexcept {
			return location(m_ptr - rhs);
		}

		[[nodiscard]] b8 operator>(const location &rhs) const noexcept {
			return reinterpret_cast<p64>(m_ptr) > reinterpret_cast<p64>(rhs.m_ptr);
		}

		[[nodiscard]] b8 operator>=(const location &rhs) const noexcept {
			return reinterpret_cast<p64>(m_ptr) >= reinterpret_cast<p64>(rhs.m_ptr);
		}

		[[nodiscard]] b8 is_aligned() const noexcept {
			return reinterpret_cast<p64>(m_ptr) % 8 == 0;
		}
	};


	constexpr sid64 ToStringId64(const char* str) noexcept {
		u64 base = 0xCBF29CE484222325;
		if (*str) {
			do {
				base = 0x100000001B3 * (base ^ *str++);
			} while (*str);
		}
		return base;
	}

	inline const std::string int_to_string_id(sid64 sid) noexcept {
		char buffer[20] = { 0 };
		std::snprintf(buffer, sizeof(buffer), "#%016llX", sid);
		return buffer;
	}

	inline const std::string offset_to_string(u32 offset) noexcept {
		char buffer[20] = { 0 };
		std::snprintf(buffer, sizeof(buffer), "0x%06X", offset);
		return buffer;
	}
};



