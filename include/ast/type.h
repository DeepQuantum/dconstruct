#pragma once

#include "base.h"
#include <map>
#include <optional>
#include <variant>
#include <vector>
#include <expected>
#include "disassembly/opcodes.h"

namespace dconstruct::ast {

    // struct darray {};
    // struct ddict {};

    using primitive_value = std::variant<u8, u16, u32, u64, i8, i16, i32, i64, f32, f64, char, bool, std::string, sid64_literal, sid32_literal, std::nullptr_t, std::monostate>;
    using primitive_number = std::variant<u8, u16, u32, u64, i8, i16, i32, i64, f32, f64, char>;


    enum class primitive_kind {
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
        SID32,
        NULLPTR,
        NOTHING
    };

    static_assert(std::variant_size_v<primitive_value> == static_cast<u64>(primitive_kind::NOTHING) + 1);

    [[nodiscard]] primitive_kind kind_from_primitive_value(const primitive_value& prim) noexcept;

    [[nodiscard]] std::optional<primitive_number> get_number(const primitive_value& prim) noexcept;


    struct primitive_type;
    struct struct_type;
    struct enum_type;
    struct ptr_type;
    struct function_type;
    struct darray;

    using full_type = std::variant<std::monostate, primitive_type, struct_type, enum_type, ptr_type, function_type>;

    using ref_full_type = std::shared_ptr<full_type>;


    [[nodiscard]] static full_type make_type_from_prim(const primitive_kind kind);

    struct primitive_type {
        primitive_kind m_type;
        bool operator==(const primitive_type&) const = default;
    };

    struct struct_type {
        std::string m_name;
        std::map<std::string, ref_full_type> m_members;
        bool operator==(const struct_type&) const = default;
    };

    struct enum_type {
        std::string m_name;
        std::vector<std::string> m_enumerators;
        bool operator==(const enum_type&) const = default;
    };

    struct function_type {
        using t_arg_list = std::vector<std::pair<std::string, ref_full_type>>;

        ref_full_type m_return;
        t_arg_list m_arguments;
        bool m_isFarCall = false;

        explicit function_type() noexcept : m_return{ std::make_shared<ast::full_type>(std::monostate()) } {};
        explicit function_type(ref_full_type return_type, t_arg_list args) noexcept : m_return{ std::move(return_type) }, m_arguments{args} {};

        bool operator==(const function_type&) const = default;
    };

    // struct darray {
    //     ref_full_type m_arrType;

    //     explicit darray(ref_full_type&& type) noexcept : m_arrType{std::move(type)}{};

    //     explicit darray(const ast::primitive_kind& kind) noexcept : m_arrType{std::make_shared<ast::full_type>(make_type_from_prim(kind))}{};

    //     bool operator==(const darray&) const = default;
    // };
    
    struct ptr_type {
        ref_full_type m_pointedAt;
        explicit ptr_type() noexcept : m_pointedAt{std::make_shared<ast::full_type>(std::monostate())}{};
        explicit ptr_type(ref_full_type&& type) noexcept : m_pointedAt{std::move(type)}{};

        explicit ptr_type(const ast::primitive_kind& kind) noexcept : m_pointedAt{std::make_shared<ast::full_type>(make_type_from_prim(kind))}{};

        bool operator==(const ptr_type&) const = default;
    };


    [[nodiscard]] inline static full_type make_type_from_prim(const primitive_kind kind) {
        return full_type{primitive_type{kind}};
    }

    [[nodiscard]] inline static bool is_unknown(const full_type& type) noexcept {
        return std::holds_alternative<std::monostate>(type);
    }

    [[nodiscard]] bool is_signed(const full_type& type) noexcept; 


    [[nodiscard]] static function_type make_function(const ast::full_type& return_arg, const std::initializer_list<std::pair<std::string, full_type>>& args) {
        ast::function_type::t_arg_list arg_types;
        for (const auto& [name, type] : args) {
            arg_types.emplace_back(name, std::make_shared<full_type>(type));
        }
        return function_type{std::make_shared<ast::full_type>(return_arg), std::move(arg_types)};
    }

    struct typed_value;

    using struct_instance = std::map<std::string, std::shared_ptr<typed_value>>;
    using enum_instance = std::string;
    using ptr_instance = std::shared_ptr<typed_value>;
    using function_instance = std::string;

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

    [[nodiscard]] std::string type_to_declaration_string(const full_type& type);

    [[nodiscard]] std::string primitive_to_string(const primitive_value& prim);

    [[nodiscard]] std::string kind_to_string(const primitive_kind kind) noexcept;

    [[nodiscard]] u64 get_size(const full_type& type) noexcept;

    [[nodiscard]] constexpr bool is_integral(primitive_kind k) noexcept {
        return
            k == primitive_kind::U8  ||
            k == primitive_kind::U16 ||
            k == primitive_kind::U32 ||
            k == primitive_kind::U64 ||
            k == primitive_kind::I8  ||
            k == primitive_kind::I16 ||
            k == primitive_kind::I32 ||
            k == primitive_kind::I64 ||
            k == primitive_kind::CHAR ||
            k == primitive_kind::BOOL;
    }

    
    [[nodiscard]] constexpr bool is_floating_point(primitive_kind k) noexcept {
        return k == primitive_kind::F32 || k == primitive_kind::F64;
    }
    
    [[nodiscard]] constexpr bool is_arithmetic(primitive_kind k) noexcept {
        return is_integral(k) || is_floating_point(k);
    }

    [[nodiscard]] constexpr bool is_unsigned(const primitive_kind k) noexcept {
        return  k == primitive_kind::U8  ||
                k == primitive_kind::U16 ||
                k == primitive_kind::U32 ||
                k == primitive_kind::U64;
    }

    [[nodiscard]] constexpr bool is_signed(const primitive_kind k) noexcept {
        return  k == primitive_kind::I8  ||
                k == primitive_kind::I16 ||
                k == primitive_kind::I32 ||
                k == primitive_kind::I64;
    }

    

    [[nodiscard]] constexpr std::optional<primitive_kind> get_dominating_prim(const primitive_kind lhs, const primitive_kind rhs) {
        auto int_rank = [](primitive_kind k) -> u32 {
            switch (k) {
                case primitive_kind::U8:
                case primitive_kind::I8:  return 8;
                case primitive_kind::U16:
                case primitive_kind::I16: return 16;
                case primitive_kind::U32:
                case primitive_kind::I32: return 32;
                case primitive_kind::U64:
                case primitive_kind::I64: return 64;
                default: return 0;
            }
        };

        auto float_rank = [](primitive_kind k) -> u32 {
            switch (k) {
                case primitive_kind::F32: return 32;
                case primitive_kind::F64: return 64;
                default: return 0;
            }
        };

        if (lhs == rhs) {
            return lhs;
        }

        const bool lhs_unsigned = is_unsigned(lhs);
        const bool rhs_unsigned = is_unsigned(rhs);
        const bool lhs_signed   = is_signed(lhs);
        const bool rhs_signed   = is_signed(rhs);

        if ((lhs_unsigned || lhs_signed) && (rhs_unsigned || rhs_signed)) {
            const u32 l = int_rank(lhs);
            const u32 r = int_rank(rhs);

            if (l < 0 || r < 0) {
                return std::nullopt;
            }

            return l >= r ? lhs : rhs;
        }

        const u32 lf = float_rank(lhs);
        const u32 rf = float_rank(rhs);

        if (lf > 0 && rf > 0) {
            return lf >= rf ? lhs : rhs;
        }

        return std::nullopt;
    }

    [[nodiscard]] static std::optional<std::string> not_assignable_reason(const full_type& assignee, const full_type& assign_value) {
        return std::visit([](auto&& lhs, auto&& rhs) -> std::optional<std::string> {
            using lhs_t = std::decay_t<decltype(lhs)>;
            using rhs_t = std::decay_t<decltype(rhs)>;
            if constexpr (std::is_same_v<lhs_t, primitive_type>) {
                if constexpr (!std::is_same_v<rhs_t, primitive_type>) {
                    return "expected type " + type_to_declaration_string(lhs) + " for assignment but got " + type_to_declaration_string(rhs);
                } else {
                    const auto new_kind = get_dominating_prim(lhs.m_type, rhs.m_type);
                    if (!new_kind) {
                        return "expected type " + type_to_declaration_string(lhs) + " for assignment but got " + type_to_declaration_string(rhs);
                    }
                    return std::nullopt;
                }
            } else if constexpr (std::is_same_v<lhs_t, rhs_t>) {
                return std::nullopt;
            } else {
                return "expected type " + type_to_declaration_string(lhs) + " for assignment but got " + type_to_declaration_string(rhs);
            }
        }, assignee, assign_value);
    }

    [[nodiscard]] static std::expected<full_type, std::string> is_valid_binary_op(const full_type& lhs, const full_type& rhs, const std::string& op) {
        return std::visit([&op](auto&& lhs, auto rhs) -> std::expected<full_type, std::string> {
            using lhs_t = std::decay_t<decltype(lhs)>;
            using rhs_t = std::decay_t<decltype(rhs)>;

            if constexpr (!std::is_same_v<lhs_t, primitive_type>) {
                return std::unexpected{"expected primitive type for '" + op + "' lhs but got " + type_to_declaration_string(lhs)};
            } else if constexpr (!std::is_same_v<rhs_t, primitive_type>) {
                return std::unexpected{"expected primitive type for '" + op + "' rhs but got " + type_to_declaration_string(rhs)};
            } else {
                const std::optional<primitive_kind> new_prim = get_dominating_prim(lhs.m_type, rhs.m_type);
                if (!new_prim) {
                    return std::unexpected{"expected compatible types for '" + op + "' but got " + type_to_declaration_string(lhs) + " and " + type_to_declaration_string(rhs)};
                } else {
                    return make_type_from_prim(*new_prim);
                }
            }
        }, lhs, rhs);
    }

    template<typename T> inline constexpr bool is_primitive = std::is_same_v<T, primitive_type>;
    template<typename T> inline constexpr bool is_pointer = std::is_same_v<T, ptr_type>;

    [[nodiscard]] inline bool operator==(const ref_full_type& lhs, const ref_full_type& rhs) noexcept {
        if (lhs == nullptr && rhs == nullptr) {
            return true;
        } else if (lhs == nullptr || rhs == nullptr) {
            return false;
        } else {
            return *lhs == *rhs;
        }
    }

    [[nodiscard]] inline bool operator!=(const ref_full_type& lhs, const ref_full_type& rhs) noexcept {
        return !(lhs == rhs);
    }

    [[nodiscard]] static inline const full_type* get_dominating_type(const full_type& lhs, const full_type& rhs) {
        std::optional<full_type> res;
        if (lhs == rhs) {
            return &lhs;
        }
    }

    [[nodiscard]] std::expected<Opcode, std::string> get_load_opcode(const full_type& type); 
    [[nodiscard]] std::expected<Opcode, std::string> get_store_opcode(const full_type& type); 
}
