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

    const std::string UNKNOWN_TYPE_NAME = "u64?";

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


    [[nodiscard]] full_type make_type_from_prim(const primitive_kind kind);

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

        explicit function_type() noexcept;
        explicit function_type(ref_full_type return_type, t_arg_list args) noexcept;

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
        explicit ptr_type() noexcept;
        explicit ptr_type(ref_full_type&& type) noexcept;

        explicit ptr_type(const ast::primitive_kind& kind) noexcept;

        bool operator==(const ptr_type&) const = default;
    };


    [[nodiscard]] full_type make_type_from_prim(const primitive_kind kind);

    [[nodiscard]] bool is_unknown(const full_type& type) noexcept;

    [[nodiscard]] bool is_signed(const full_type& type) noexcept; 


    [[nodiscard]] function_type make_function(const ast::full_type& return_arg, const std::initializer_list<std::pair<std::string, full_type>>& args);

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

    [[nodiscard]] bool is_integral(primitive_kind k) noexcept;

    
    [[nodiscard]] bool is_floating_point(primitive_kind k) noexcept;
    
    [[nodiscard]] bool is_arithmetic(primitive_kind k) noexcept;

    [[nodiscard]] bool is_unsigned(const primitive_kind k) noexcept;

    [[nodiscard]] bool is_signed(const primitive_kind k) noexcept;

    

    [[nodiscard]] std::optional<primitive_kind> get_dominating_prim(const primitive_kind lhs, const primitive_kind rhs);

    [[nodiscard]] std::optional<std::string> not_assignable_reason(const full_type& assignee, const full_type& assign_value);

    [[nodiscard]] std::expected<full_type, std::string> is_valid_binary_op(const full_type& lhs, const full_type& rhs, const std::string& op);

    template<typename T> inline constexpr bool is_primitive = std::is_same_v<T, primitive_type>;
    template<typename T> inline constexpr bool is_pointer = std::is_same_v<T, ptr_type>;

    [[nodiscard]] bool operator==(const ref_full_type& lhs, const ref_full_type& rhs) noexcept;

    [[nodiscard]] bool operator!=(const ref_full_type& lhs, const ref_full_type& rhs) noexcept;

    [[nodiscard]] const full_type* get_dominating_type(const full_type& lhs, const full_type& rhs);

    [[nodiscard]] std::expected<Opcode, std::string> get_load_opcode(const full_type& type); 
    [[nodiscard]] std::expected<Opcode, std::string> get_store_opcode(const full_type& type); 

    extern const std::string UNKNOWN_TYPE_NAME;
}
