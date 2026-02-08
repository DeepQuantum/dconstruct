#include "ast/type.h"
#include <sstream>
#include <numeric>

namespace dconstruct::ast {

[[nodiscard]] primitive_kind kind_from_primitive_value(const primitive_value& prim) noexcept {
    return static_cast<primitive_kind>(prim.index());
}

[[nodiscard]] std::optional<primitive_number> get_number(const primitive_value& prim) noexcept {
    return std::visit([](auto&& arg) -> std::optional<primitive_number> {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_arithmetic_v<T>) {
            return static_cast<primitive_number>(arg);
        } else {
            return std::nullopt;
        }
    }, prim);
}

[[nodiscard]] bool is_signed(const ast::full_type& type) noexcept {
    return std::visit([](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, ast::primitive_type>) {
            return arg.m_type == primitive_kind::I8 || arg.m_type == primitive_kind::I16 || arg.m_type == primitive_kind::I32 || arg.m_type == primitive_kind::I64;
        } else {
            return false;
        }
    }, type);
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
        else if constexpr (std::is_same_v<T, sid32_literal> || std::is_same_v<T, sid64_literal>) {
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
        case primitive_kind::U8:        return "u8";
        case primitive_kind::U16:       return "u16";
        case primitive_kind::U32:       return "u32";
        case primitive_kind::U64:       return "u64";
        case primitive_kind::I8:        return "i8";
        case primitive_kind::I16:       return "i16";
        case primitive_kind::I32:       return "i32";
        case primitive_kind::I64:       return "i64";
        case primitive_kind::F32:       return "f32";
        case primitive_kind::F64:       return "f64";
        case primitive_kind::CHAR:      return "char";
        case primitive_kind::BOOL:      return "bool";
        case primitive_kind::STRING:    return "string";
        case primitive_kind::SID:       return "sid";
        case primitive_kind::NULLPTR:   return "nullptr";
        default:                        return "u64?";
    }
}

[[nodiscard]] std::string type_to_declaration_string(const full_type& type) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, struct_type> || std::is_same_v<T, enum_type>) {
            return arg.m_name;
        } else if constexpr (std::is_same_v<T, ptr_type>) {
            return type_to_declaration_string(*arg.m_pointedAt) + '*';
        } else if constexpr (std::is_same_v<T, function_type>) {
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
            return "u64?";
        } else if constexpr(std::is_same_v<T, darray>) {
            return "darray<" + type_to_declaration_string(*arg.m_arrType) + ">";
        } else {
            return kind_to_string(arg.m_type);
        }
    }, type);
}

[[nodiscard]] u64 get_size(const full_type& type) noexcept {
    return std::visit([](auto&& arg) -> u64 {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, primitive_type>) {
            switch (arg.m_type) {
                case primitive_kind::U8: case primitive_kind::I8: case primitive_kind::CHAR: case primitive_kind::BOOL: return 1;
                case primitive_kind::U16: case primitive_kind::I16: return 2;
                case primitive_kind::U32: case primitive_kind::I32: case primitive_kind::F32: case primitive_kind::SID32: return 4;
                case primitive_kind::U64: case primitive_kind::I64: case primitive_kind::F64: case primitive_kind::STRING: case primitive_kind::SID: return 8;
                default: return 0;
            }
        } else if constexpr (std::is_same_v<T, struct_type>) {
            return std::accumulate(arg.m_members.begin(), arg.m_members.end(), u64{0}, [](u64 acc, const auto& member) {
                return acc + get_size(*member.second.get());
            });
        } else if constexpr (std::is_same_v<T, enum_type>) {
            return sizeof(u64);
        } else if constexpr (std::is_same_v<T, ptr_type>) {
            return sizeof(void*);
        } else if constexpr (std::is_same_v<T, function_type>) {
            return sizeof(void*);
        } else if constexpr (std::is_same_v<T, std::monostate>) {
            return 0;
        } else if constexpr (std::is_same_v<T, darray>) {
            return get_size(*arg.m_arrType.get());
        } else {
            return 0;
        }
    }, type);
}


[[nodiscard]] std::expected<Opcode, std::string> get_load_opcode(const full_type& type) {
    return std::visit([](auto&& arg) -> std::expected<Opcode, std::string> {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, primitive_type>) {
            const primitive_kind kind = arg.m_type;
            switch (kind) {
                case primitive_kind::I8:  return Opcode::LoadI8;
                case primitive_kind::U8:  return Opcode::LoadU8;
                case primitive_kind::I16: return Opcode::LoadI16;
                case primitive_kind::U16: return Opcode::LoadU16;
                case primitive_kind::I32: return Opcode::LoadI32;
                case primitive_kind::U32: return Opcode::LoadU32;
                case primitive_kind::I64: return Opcode::LoadI64;
                case primitive_kind::U64: return Opcode::LoadU64;
                case primitive_kind::F32: return Opcode::LoadFloat;
                default: return std::unexpected{"no load opcode for primitive type " + type_to_declaration_string(arg)};
            }
        } else if constexpr (std::is_same_v<T, ptr_type>) {
            return Opcode::LoadPointer;
        } else {
            return std::unexpected{"no load opcode for type " + type_to_declaration_string(arg)};
        }
    }, type);
}

[[nodiscard]] std::expected<Opcode, std::string> get_store_opcode(const full_type& type) {
    return std::visit([](auto&& arg) -> std::expected<Opcode, std::string> {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, primitive_type>) {
            const primitive_kind kind = arg.m_type;
            switch (kind) {
                case primitive_kind::I8:  return Opcode::StoreI8;
                case primitive_kind::U8:  return Opcode::StoreU8;
                case primitive_kind::I16: return Opcode::StoreI16;
                case primitive_kind::U16: return Opcode::StoreU16;
                case primitive_kind::I32: return Opcode::StoreI32;
                case primitive_kind::U32: return Opcode::StoreU32;
                case primitive_kind::I64: return Opcode::StoreI64;
                case primitive_kind::U64: return Opcode::StoreU64;
                case primitive_kind::F32: return Opcode::StoreFloat;
                default: return std::unexpected{"no store opcode for primitive type " + type_to_declaration_string(arg)};
            }
        } else if constexpr (std::is_same_v<T, ptr_type>) {
            return Opcode::StorePointer;
        } else {
            return std::unexpected{"no store opcode for type " + type_to_declaration_string(arg)};
        }
    }, type);
}

}