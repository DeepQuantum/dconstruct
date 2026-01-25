#pragma once

#include <string>
#include <cstdio>
#include <cstdint>
#include <memory>
#include <optional>
#include <bitset>
#include <vector>
#include <sstream>
#include <bit>
#include <cassert>

using sid64 = uint64_t;
using sid32 = uint32_t;


using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;
using p64 = uintptr_t;



namespace dconstruct {

	using sid64_literal = std::pair<sid64, std::string>;
	using sid32_literal = std::pair<sid32, std::string>;

	using reg_idx = u8;
	using node_id = u16;
	using istr_line = u16;

	constexpr reg_idx ARGUMENT_REGISTERS_IDX = 49;
	constexpr reg_idx MAX_REGISTER = ARGUMENT_REGISTERS_IDX * 2;

	using reg_set = std::bitset<ARGUMENT_REGISTERS_IDX + 1>;
	using argument_reg_set = std::bitset<ARGUMENT_REGISTERS_IDX + 1>;
	using node_set = std::vector<bool>;

	#define SID(str) (dconstruct::ToStringId64(str))

	static std::string sanitize_dc_string(const std::string &dc_string) {
		std::string sanitized;
		sanitized.reserve(dc_string.size());
		for (char c : dc_string) {
			switch (c) {
				case '?':
				case '>':
				case '<':
				case '*':
				case '\\':
				case '/':
				case '|':
				case '\"':
				case ':':
				case '@':
				case '-': {
					sanitized += '_';
					break;
				}
				default: {
					sanitized += c;
				}
			}
		}
		return sanitized;
	}

	[[nodiscard]] static inline std::optional<std::string> try_convert_pascal_case(const std::string& orig) noexcept {
		if (orig.size() < 3) {
			return std::nullopt;
		}

		std::string res;
		res.reserve(orig.size());

		// foo-bar -> FooBar

		if (orig[0] >= 97 && orig[0] <= 122) {
			res += orig[0] - 32;
		} else {
			res += orig[0];
		}

		for (u64 i = 1; i < orig.size(); ++i) {
			if (orig[i] == '-' && orig[i + 1] >= 97 && orig[i + 1] <= 122) {
				res += orig[i + 1] - 32;
				i += 1;
			} else {
				res += orig[i];
			}
		}

		return res;
	}

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

		[[nodiscard]] bool operator>(const location &rhs) const noexcept {
			return reinterpret_cast<p64>(m_ptr) > reinterpret_cast<p64>(rhs.m_ptr);
		}

		[[nodiscard]] bool operator>=(const location &rhs) const noexcept {
			return reinterpret_cast<p64>(m_ptr) >= reinterpret_cast<p64>(rhs.m_ptr);
		}

		[[nodiscard]] bool is_aligned() const noexcept {
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

	constexpr sid32 ToStringId32(const char* str) noexcept {
		u32 base = 0x811c9dc5;
		if (*str) {
			do {
				base = 0x811c9dc5  * (base ^ *str++);
			} while (*str);
		}
		return base;
	}

	template<typename T> requires (std::is_same_v<T, sid32> || std::is_same_v<T,sid64>)
	inline const std::string int_to_string_id(T sid) noexcept {
		char buffer[20] = { 0 };
		if constexpr (sizeof(T) == 4) {
			std::snprintf(buffer, sizeof(buffer), "#%08lX", sid);
		} else {
			std::snprintf(buffer, sizeof(buffer), "#%016llX", sid);
		}
		return buffer;
	}

	inline const std::string offset_to_string(u32 offset) noexcept {
		char buffer[20] = { 0 };
		std::snprintf(buffer, sizeof(buffer), "0x%06X", offset);
		return buffer;
	}

	[[nodiscard]] static inline std::string pretty_regset(reg_set set) {
		std::stringstream ss;
		ss << '[';
		auto bits = set.to_ullong();
		while (bits != 0) {
			ss << std::countr_zero(bits) << ", ";
			bits &= bits - 1;
		}
		ss << ']';
		return ss.str();
	}
};