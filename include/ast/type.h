#pragma once

#include "base.h"
#include <map>
#include <optional>
#include <variant>
#include <vector>
#include "disassembly/instructions.h"

namespace dconstruct::ast {

    
    using primitive_value = std::variant<u8, u16, u32, u64, i8, i16, i32, i64, f32, f64, char, b8, std::string, sid_literal>;
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
        _NULL,
    };

    [[nodiscard]] primitive_kind kind_from_primitive_value(const primitive_value& prim) noexcept;

    [[nodiscard]] std::optional<primitive_number> get_number(const primitive_value& prim) noexcept;

    struct primitive_type;
    struct struct_type;
    struct enum_type;
    struct ptr_type;
    struct function_type;

    enum full_type_kind : u8 {
        UNKNOWN_TYPE,
        PRIMITIVE_TYPE,
        STRUCT_TYPE,
        ENUM_TYPE,
        PTR_TYPE,
        FUNCTION_TYPE,
    };

    using full_type_base = std::variant<std::monostate, primitive_type, struct_type, enum_type, ptr_type, function_type>;
    
    struct full_type : full_type_base  {
        using full_type_base::full_type_base;
        
        full_type() = default;

        full_type(const primitive_kind& kind) noexcept : full_type_base{primitive_type{kind}}{};
    };

    struct primitive_type {
        primitive_kind m_type;
        bool operator==(const primitive_type&) const = default;
    };

    struct struct_type {
        std::string m_name;
        std::map<std::string, std::shared_ptr<full_type>> m_members;
        bool operator==(const struct_type&) const = default;
    };

    struct enum_type {
        std::string m_name;
        std::vector<std::string> m_enumerators;
        bool operator==(const enum_type&) const = default;
    };

    struct ptr_type {
        std::shared_ptr<full_type> m_pointedAt;
        explicit ptr_type() noexcept : m_pointedAt{std::make_shared<ast::full_type>(std::monostate())}{};

        explicit ptr_type(const ast::primitive_kind& kind) noexcept : m_pointedAt{std::make_shared<ast::full_type>(kind)}{};

        explicit ptr_type(ast::full_type&& type) noexcept : m_pointedAt{std::make_shared<ast::full_type>(std::move(type))}{};
        bool operator==(const ptr_type&) const = default;
    };

    struct function_type {
        full_type m_return;
        std::map<std::string, std::shared_ptr<full_type>> m_arguments;
    };

    struct typed_value;

    using struct_instance = std::map<std::string, typed_value>;
    using enum_instance = std::string;
    using ptr_instance = std::shared_ptr<typed_value>;
    using function_instance = std::string;

    using value_variant = std::variant<
        std::monostate,
        primitive_value,
        struct_instance,
        enum_instance,
        ptr_instance,
        function_instance,
    >;

    struct typed_value {
        full_type type;
        value_variant value;
    };

    [[nodiscard]] std::string type_to_declaration_string(const full_type& type);

    [[nodiscard]] std::string primitive_to_string(const primitive_value& prim);

    [[nodiscard]] std::string kind_to_string(const primitive_kind kind) noexcept;
}
