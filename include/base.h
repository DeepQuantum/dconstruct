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
#include <expected>
#include <filesystem>


#ifdef __INTELLISENSE__
#  define DCONSTRUCT_CTRE_LSP_NUKE_GUARD(P) ""
#else
#  define DCONSTRUCT_CTRE_LSP_NUKE_GUARD(P) P
#endif


#if defined(_MSC_VER)
#define DC_ALWAYS_INLINE __forceinline
#elif defined(__GNUC__)
#define DC_ALWAYS_INLINE [[gnu::always_inline]] inline
#else
#define DC_ALWAYS_INLINE inline
#endif

#define SID(str) (dconstruct::ToStringId64(str))

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)
#define RETURN_UNEXP(decl, expr)                                            \
    auto CONCAT(_tmp_, __LINE__) = (expr);                                  \
    if (!CONCAT(_tmp_, __LINE__).has_value())                               \
        return std::unexpected(std::move(CONCAT(_tmp_, __LINE__).error())); \
    decl = std::move(*CONCAT(_tmp_, __LINE__))

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

#undef max

namespace dconstruct {

    using sid64_literal = std::pair<sid64, std::string>;
    using sid32_literal = std::pair<sid32, std::string>;

    template<typename R, typename T>
    using res = std::expected<R, T>;

    template<typename T>
    using resstr = res<T, std::string>;

    using errmsg = std::optional<std::string>;

    using reg_idx = u8;
    using node_id = u16;
    using istr_line = u16;

    constexpr reg_idx ARGUMENT_REGISTERS_IDX = 49;
    constexpr reg_idx MAX_REGISTER = ARGUMENT_REGISTERS_IDX * 2;

    using reg_set = std::bitset<ARGUMENT_REGISTERS_IDX + 1>;
    using argument_reg_set = std::bitset<ARGUMENT_REGISTERS_IDX + 1>;
    using node_set = std::vector<bool>;

    [[nodiscard]] std::filesystem::path executable_path();

    [[nodiscard]] inline std::filesystem::path executable_relative_sidbase() {
        return executable_path().parent_path() / "sidbase.bin";
    }

    [[nodiscard]] inline std::filesystem::path executable_relative_var_maps() {
        return executable_path().parent_path() / "var_maps";
    }

    static std::string sanitize_dc_string(const std::string& dc_string) {
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

        [[nodiscard]] DC_ALWAYS_INLINE location& from(const location& rhs, const i32 offset = 0) noexcept {
            m_ptr = rhs.get<std::byte*>() + offset;
            return *this;
        }

        template <typename T>
        [[nodiscard]] DC_ALWAYS_INLINE const T* as(const i32 offset = 0) const noexcept {
            return reinterpret_cast<const T*>(m_ptr + offset);
        }

        template <typename T>
        [[nodiscard]] DC_ALWAYS_INLINE const T& get(const i32 offset = 0) const noexcept {
            return *reinterpret_cast<const T*>(m_ptr + offset);
        }

        [[nodiscard]] DC_ALWAYS_INLINE p64 num() const noexcept {
            return reinterpret_cast<p64>(m_ptr);
        }

        [[nodiscard]] DC_ALWAYS_INLINE location aligned() const noexcept {
            return location(m_ptr - num() % 8);
        }

        [[nodiscard]] DC_ALWAYS_INLINE location operator+(const u64 rhs) const noexcept {
            return location(m_ptr + rhs);
        }

        [[nodiscard]] DC_ALWAYS_INLINE location operator-(const u64 rhs) const noexcept {
            return location(m_ptr - rhs);
        }

        [[nodiscard]]DC_ALWAYS_INLINE bool operator>(const location& rhs) const noexcept {
            return reinterpret_cast<p64>(m_ptr) > reinterpret_cast<p64>(rhs.m_ptr);
        }

        [[nodiscard]] DC_ALWAYS_INLINE bool operator>=(const location& rhs) const noexcept {
            return reinterpret_cast<p64>(m_ptr) >= reinterpret_cast<p64>(rhs.m_ptr);
        }

        [[nodiscard]] DC_ALWAYS_INLINE bool is_aligned() const noexcept {
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
                base = 0x811c9dc5 * (base ^ *str++);
            } while (*str);
        }
        return base;
    }

    template <typename T>
        requires(std::is_same_v<T, sid32> || std::is_same_v<T, sid64>)
    inline const std::string int_to_string_id(T sid) noexcept {
        char buffer[20] = {0};
        if constexpr (sizeof(T) == 4) {
            std::snprintf(buffer, sizeof(buffer), "#%08lX", sid);
        } else {
            std::snprintf(buffer, sizeof(buffer), "#%016llX", sid);
        }
        return buffer;
    }

    inline const std::string offset_to_string(u32 offset) noexcept {
        char buffer[20] = {0};
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

    constexpr reg_idx operator""_r(u64 v) {
        return reg_idx(v);
    }

    constexpr u8 operator""_imm(u64 v) {
        return u8(v);
    }

    constexpr std::byte operator""_b(u64 v) {
        return std::byte(v);
    }

    // once C++26 is added to MSVC....
    /*

    template<typename T>
    [[nodiscard]] static consteval std::pair<u64, u64> get_bitmap() {
        u64 res = 0;
        u64 count = 0;
        u64 size = 0;
        template for (constexpr auto mem : define_static_array(nonstatic_data_members_of(^^T, std::meta::access_context::current()))) {
            using member_t = [:std::meta::type_of(mem):];
            size += sizeof(member_t);
            if (size % sizeof(u64) == 0) {
                res |= static_cast<u64>(is_pointer_type(type_of(mem))) << count++;
            }
        }
        return {size, res};
    }

    */
};
