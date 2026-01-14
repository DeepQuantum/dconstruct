#pragma once

#include "base.h"
#include <map>
#include <optional>
#include <variant>
#include <vector>

namespace dconstruct::ast {
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
        NULLPTR,
        NOTHING
    };

    [[nodiscard]] primitive_kind kind_from_primitive_value(const primitive_value& prim) noexcept;

    [[nodiscard]] std::optional<primitive_number> get_number(const primitive_value& prim) noexcept;


    struct primitive_type;
    struct struct_type;
    struct enum_type;
    struct ptr_type;
    struct function_type;

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

        explicit function_type() noexcept : m_return{ std::make_shared<ast::full_type>(std::monostate()) } {};
        explicit function_type(ref_full_type return_type, t_arg_list args) noexcept : m_return{ std::move(return_type) }, m_arguments{args} {};
    };
    
    struct ptr_type {
        ref_full_type m_pointedAt;
        explicit ptr_type() noexcept : m_pointedAt{std::make_shared<ast::full_type>(std::monostate())}{};

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
}
