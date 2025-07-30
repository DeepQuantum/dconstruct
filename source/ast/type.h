#pragma once

#include "base.h"
#include "tokens.h"


namespace dconstruct::ast {

    using type_id = u32;

    enum type_kind : u8 {
        TK_UNKNOWN,
        TK_PRIMITIVE,
        TK_STRUCT,
        TK_ENUM,
    };

    struct primitive_type { 
        enum tag : u8 {
            PRIM_U8,
            PRIM_U16,
            PRIM_U32,
            PRIM_U64,
            PRIM_I8,
            PRIM_I16,
            PRIM_I32,
            PRIM_I64,
            PRIM_F32,
            PRIM_F64,
            PRIM_CHAR,
            PRIM_BOOL,
            PRIM_STRING,
            PRIM_SID
        } tag;
    };

    struct field {
        std::string m_name;
        type_id m_type;
    };

    struct struct_type {
        std::string m_name;
        std::vector<field> m_fields;
    };
    
    struct enum_type {
        std::string m_name;
        std::vector<std::string> m_enumerators;
    };

    using type = std::variant<primitive_type, struct_type, enum_type>;



    struct custom_type;

    struct type : public std::variant<primitive, custom_type> {
        using base = std::variant<primitive, custom_type>;
    };

    struct custom_type : public Iprintable {
        std::string m_name;
        u32 m_size;
        std::vector<type> m_members;
    };

    struct type {
        std::string to_string() const noexcept {
            if (m_value.index() == 0) {
                return std::get<std::string>(m_value);
            }
            else {
                switch (std::get<primitive>(m_value)) {
                    case E_UNKNOWN: return "var";
                    case E_INT: return "i32";
                    case E_FLOAT: return "f32";
                    case E_STRING: return "string";
                    case E_PTR: return "p64";
                    case E_HASH: return "sid64";
                    default: return "var";
                }
            }
        }
    };
}
