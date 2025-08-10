#pragma once

#include "base.h"
#include "compilation/tokens.h"
#include <map>
#include <optional>
#include <vector>

namespace dconstruct::ast {

    using sid_literal_type = std::pair<sid64, std::string>;
    using primitive_value_type = std::variant<u8, u16, u32, u64, i8, i16, i32, i64, f32, f64, char, b8, std::string, sid_literal_type>;
    using primitive_number_type = std::variant<u8, u16, u32, u64, i8, i16, i32, i64, f32, f64, char>;

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

    inline type_kind kind_from_primitive_value(const primitive_value_type& prim) noexcept {
        return static_cast<type_kind>(prim.index());
    }

    [[nodiscard]] inline std::optional<primitive_number_type> get_number(const primitive_value_type& prim) noexcept {
        return std::visit([](auto&& arg) -> std::optional<primitive_number_type> {
            if constexpr (std::is_arithmetic_v<std::decay_t<decltype(arg)>>) {
                return static_cast<primitive_number_type>(arg);
            } else {
                return std::nullopt;
            }
        }, prim);
    }

    inline b8 is_primitive(const type_kind kind) noexcept {
        return kind >= type_kind::U8 && kind <= type_kind::SID;
    }

    struct struct_type {
        std::string m_name;
        std::map<std::string, type_kind> m_members;
    };

    struct enum_type {
        std::string m_name;
        std::vector<std::string> m_enumerators;
    };

    struct ptr_type {
        type_kind m_pointedAt;
    };

    using full_type = std::variant<struct_type, enum_type, ptr_type, primitive_value_type>;
    using nullable_type = std::variant<full_type, nullptr_t>;

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

    /*struct primitive : public Iprintable {
        using primitive_value_type = std::variant<u8,u16,u32,u64,i8,i16,i32,i64,f32,f64,char,b8,std::string,sid_literal_type,nullptr_t>;
        template<typename T>
        primitive(T&& v) noexcept : m_value(std::forward<T>(v)), m_kind(static_cast<primitive_kind>(m_value.index())) {}

        primitive() : m_value(nullptr), m_kind(PK_UNKNOWN) {};
    
        inline void pseudo(std::ostream& os) const final {
            os << value_to_string();
        }

        inline void ast(std::ostream& os) const final {
            os << "primitive[";
            std::visit([&](auto&& arg) -> void {
                os << typeid(arg).name() << ": " << value_to_string();
            }, m_value);
            os << "]";
        }

        const primitive_value_type& get_value() const noexcept {
            return m_value;
        }

    private:
        primitive_value_type m_value;
        primitive_kind m_kind;

        constexpr std::string value_to_string() const {
            return std::visit([](auto&& arg) -> std::string {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    return arg;
                } else if constexpr (std::is_same_v<T, char>) {
                    return std::string(1, arg);
                } else if constexpr (std::is_same_v<T, b8>) {
                    return arg ? "true" : "false";
                } else if constexpr (std::is_same_v<T, nullptr_t>) {
                    return "null";
                } else if constexpr (std::is_same_v<T, sid_literal_type>) {
                    return "#" + std::get<1>(arg);
                } else {
                    return std::to_string(arg);
                }
            }, m_value);
        }
    };

    [[nodiscard]] inline b8 operator==(const primitive& lhs, const primitive& rhs) {
        return lhs.get_value() == rhs.get_value();
    }

    struct field : public Iprintable {
        std::string m_name;
        type m_type;

        inline void pseudo(std::ostream& os) const final {
            os << "";
        }

        inline void ast(std::ostream& os) const final {
            os << "field[" << m_name << ',' << m_type << ']';
        }
    };

    struct struct_type : public Iprintable {
        std::string m_name;
        std::vector<field> m_fields;

        inline void pseudo(std::ostream& os) const final {
            os << "";
        }

        inline void ast(std::ostream& os) const final {
            os << "struct[" << m_name << ',{';
            for (const auto& field : m_fields) {
                os << field << ';';
            }
            os << "}]";
        }
    };
    
    struct enum_type : public Iprintable {
        std::string m_name;
        std::vector<std::string> m_enumerators;

        inline void pseudo(std::ostream& os) const final {
            os << "";
        }

        inline void ast(std::ostream& os) const final {
            os << "enum[" << m_name << ',{';
            for (const auto& enumerator : m_enumerators) {
                os << enumerator << ';';
            }
            os << "}]";
        }
    };

    struct type;

    struct ptr_type : public Iprintable {
        type m_pointedAt;
    };

    struct type : public std::variant<primitive, struct_type, enum_type, ptr_type>, public Iprintable {
        using base = std::variant<primitive, struct_type, enum_type, ptr_type>;

        inline void ast(std::ostream& os) const final {
            os << "";
        }

        inline void pseudo(std::ostream& os) const final {
            std::visit([&](auto&& arg) -> void {
                os << arg;
            }, *this);
        }
    };*/
}
