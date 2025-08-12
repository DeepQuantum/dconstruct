#pragma once

#include "base.h"
#include "compilation/tokens.h"
#include <map>
#include <optional>
#include <vector>

namespace dconstruct::ast {

    using sid_literal = std::pair<sid64, std::string>;
    using primitive_value = std::variant<u8, u16, u32, u64, i8, i16, i32, i64, f32, f64, char, b8, std::string, sid_literal>;
    using primitive_number = std::variant<u8, u16, u32, u64, i8, i16, i32, i64, f32, f64, char>;

    enum class type_kind {
        U8,
        U16,
        U32,
        U64,
        I8,
        I16,
        I32,
        I64,
        F32,
        F64,
        CHAR,
        BOOL,
        STRING,
        SID,
        ENUM,
        STRUCT,
        PTR,
        _NULL,
        UNKNOWN,
    };

    inline type_kind kind_from_primitive_value(const primitive_value& prim) noexcept {
        return static_cast<type_kind>(prim.index());
    }

    [[nodiscard]] inline std::optional<primitive_number> get_number(const primitive_value& prim) noexcept {
        return std::visit([](auto&& arg) -> std::optional<primitive_number> {
            if constexpr (std::is_arithmetic_v<std::decay_t<decltype(arg)>>) {
                return static_cast<primitive_number>(arg);
            } else {
                return std::nullopt;
            }
        }, prim);
    }

    inline b8 is_primitive(const type_kind kind) noexcept {
        return kind >= type_kind::U8 && kind <= type_kind::SID;
    }

    struct primitive_type;
    struct struct_type;
    struct enum_type;
    struct ptr_type;

    using full_type = std::variant<struct_type, enum_type, ptr_type, primitive_type>;

    struct primitive_type {
        type_kind m_type;
        bool operator==(const primitive_type&) const = default;
    };

    struct struct_type {
        std::string m_name;
        std::map<std::string, type_kind> m_members;
        bool operator==(const struct_type&) const = default;
    };

    struct enum_type {
        std::string m_name;
        std::vector<std::string> m_enumerators;
        bool operator==(const enum_type&) const = default;
    };

    struct ptr_type {
        type_kind m_pointedAt;
        bool operator==(const ptr_type&) const = default;
    };

    struct typed_value;

    using struct_instance = std::map<std::string, typed_value>;
    using enum_instance = std::string;
    using ptr_instance = std::shared_ptr<typed_value>;

    using value_variant = std::variant<
        std::monostate,
        primitive_value,
        struct_instance,
        enum_instance,
        ptr_instance
    >;

    struct typed_value {
        full_type type;
        value_variant value;
    };

    inline std::string kind_to_string(const type_kind kind) noexcept {
        switch(kind) {
            case type_kind::U8:     return "u8";
            case type_kind::U16:    return "u16";
            case type_kind::U32:    return "u32";
            case type_kind::U64:    return "u64";
            case type_kind::I8:     return "i8";
            case type_kind::I16:    return "i16";
            case type_kind::I32:    return "i32";
            case type_kind::I64:    return "i64";
            case type_kind::F32:    return "f32";
            case type_kind::F64:    return "f64";
            case type_kind::CHAR:   return "char";
            case type_kind::BOOL:   return "bool";
            case type_kind::STRING: return "string";
            case type_kind::SID:    return "sid";
            case type_kind::_NULL:  return "null";
            default:                return "non-primitive";
        }
    }

}
