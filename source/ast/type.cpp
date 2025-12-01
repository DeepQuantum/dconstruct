#include "ast/type.h"
#include "disassembly/instructions.h"
#include <sstream>

namespace dconstruct::ast {

[[nodiscard]] primitive_kind kind_from_primitive_value(const primitive_value& prim) noexcept {
    return static_cast<primitive_kind>(prim.index());
}

[[nodiscard]] std::optional<primitive_number> get_number(const primitive_value& prim) noexcept {
    return std::visit([](auto&& arg) -> std::optional<primitive_number> {
        if constexpr (std::is_arithmetic_v<std::decay_t<decltype(arg)>>) {
            return static_cast<primitive_number>(arg);
        } else {
            return std::nullopt;
        }
    }, prim);
}

[[nodiscard]] std::string primitive_to_string(const primitive_value& prim) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        }
        else if constexpr (std::is_same_v<T, char>) {
            return std::string(1, arg);
        }
        else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        }
        else if constexpr (std::is_same_v<T, std::monostate>) {
            return "null";
        }
        else if constexpr (std::is_same_v<T, sid_literal>) {
            return std::get<1>(arg);
        }
        else if constexpr (std::is_same_v<T, f32>) {
            const std::string first = std::to_string(arg);
            return first.substr(0, first.find(".") + 3);
        }
        else if constexpr (std::is_same_v<T, std::nullptr_t>) {
            return "nullptr";
        }
        else {
            return std::to_string(arg);
        }
    }, prim);
}

[[nodiscard]] std::string kind_to_string(const primitive_kind kind) noexcept {
    switch(kind) {
        case primitive_kind::U8:     return "u8";
        case primitive_kind::U16:    return "u16";
        case primitive_kind::U32:    return "u32";
        case primitive_kind::U64:    return "u64";
        case primitive_kind::I8:     return "i8";
        case primitive_kind::I16:    return "i16";
        case primitive_kind::I32:    return "i32";
        case primitive_kind::I64:    return "i64";
        case primitive_kind::F32:    return "f32";
        case primitive_kind::F64:    return "f64";
        case primitive_kind::CHAR:   return "char";
        case primitive_kind::BOOL:   return "bool";
        case primitive_kind::STRING: return "string";
        case primitive_kind::SID:    return "sid";
        case primitive_kind::NULLPTR:   return "nullptr";
        default:                     return "unknown";
    }
}

[[nodiscard]] std::string type_to_declaration_string(const full_type& type) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, struct_type> || std::is_same_v<T, enum_type>) {
            return arg.m_name;
        } else if constexpr(std::is_same_v<T, ptr_type>) {
            return type_to_declaration_string(*arg.m_pointedAt) + '*';
        } else if constexpr(std::is_same_v<T, function_type>) {
            std::ostringstream os;
            os << "(";
            for (u32 i = 0; i < arg.m_arguments.size(); ++i) {
                os << type_to_declaration_string(*arg.m_arguments[i].second.get());
                if (i < arg.m_arguments.size() - 1) {
                    os << ", ";
                }
            }
            os << ") -> " << (arg.m_return != nullptr ? type_to_declaration_string(*arg.m_return.get()) : "void");
            return os.str();
        } else if constexpr(std::is_same_v<T, std::monostate>) {
            return "unknown";
        } else {
            return kind_to_string(arg.m_type);
        }
    }, type);
}


}