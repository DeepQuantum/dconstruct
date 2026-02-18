#include "ast/type.h"
#include <sstream>
#include <numeric>

namespace dconstruct::ast {

function_type::function_type() noexcept : m_return{ std::make_shared<ast::full_type>(std::monostate()) } {}

function_type::function_type(ref_full_type return_type, t_arg_list args) noexcept
    : m_return{ std::move(return_type) }, m_arguments{ std::move(args) } {}

ptr_type::ptr_type() noexcept : m_pointedAt{ std::make_shared<ast::full_type>(std::monostate()) } {}

ptr_type::ptr_type(ref_full_type&& type) noexcept : m_pointedAt{ std::move(type) } {}

ptr_type::ptr_type(const ast::primitive_kind& kind) noexcept
    : m_pointedAt{ std::make_shared<ast::full_type>(make_type_from_prim(kind)) } {}

[[nodiscard]] full_type make_type_from_prim(const primitive_kind kind) {
    return full_type{ primitive_type{ kind } };
}

[[nodiscard]] bool is_unknown(const full_type& type) noexcept {
    return std::holds_alternative<std::monostate>(type);
}

[[nodiscard]] function_type make_function(const ast::full_type& return_arg, const std::initializer_list<std::pair<std::string, full_type>>& args) {
    ast::function_type::t_arg_list arg_types;
    for (const auto& [name, type] : args) {
        arg_types.emplace_back(name, std::make_shared<full_type>(type));
    }
    return function_type{ std::make_shared<ast::full_type>(return_arg), std::move(arg_types) };
}

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

[[nodiscard]] bool is_integral(primitive_kind k) noexcept {
    return
        k == primitive_kind::U8 ||
        k == primitive_kind::U16 ||
        k == primitive_kind::U32 ||
        k == primitive_kind::U64 ||
        k == primitive_kind::I8 ||
        k == primitive_kind::I16 ||
        k == primitive_kind::I32 ||
        k == primitive_kind::I64 ||
        k == primitive_kind::CHAR ||
        k == primitive_kind::BOOL;
}

[[nodiscard]] bool is_floating_point(primitive_kind k) noexcept {
    return k == primitive_kind::F32 || k == primitive_kind::F64;
}

[[nodiscard]] bool is_arithmetic(primitive_kind k) noexcept {
    return is_integral(k) || is_floating_point(k);
}

[[nodiscard]] bool is_unsigned(const primitive_kind k) noexcept {
    return k == primitive_kind::U8 ||
           k == primitive_kind::U16 ||
           k == primitive_kind::U32 ||
           k == primitive_kind::U64;
}

[[nodiscard]] bool is_signed(const primitive_kind k) noexcept {
    return k == primitive_kind::I8 ||
           k == primitive_kind::I16 ||
           k == primitive_kind::I32 ||
           k == primitive_kind::I64;
}

[[nodiscard]] std::optional<primitive_kind> get_dominating_prim(const primitive_kind lhs, const primitive_kind rhs) {
    auto int_rank = [](primitive_kind k) -> u32 {
        switch (k) {
            case primitive_kind::U8:
            case primitive_kind::I8: return 8;
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
    const bool lhs_signed = is_signed(lhs);
    const bool rhs_signed = is_signed(rhs);

    if ((lhs_unsigned || lhs_signed) && (rhs_unsigned || rhs_signed)) {
        const u32 l = int_rank(lhs);
        const u32 r = int_rank(rhs);
        return l >= r ? lhs : rhs;
    }

    const u32 lf = float_rank(lhs);
    const u32 rf = float_rank(rhs);

    if (lf > 0 && rf > 0) {
        return lf >= rf ? lhs : rhs;
    }

    return std::nullopt;
}

[[nodiscard]] std::optional<std::string> not_assignable_reason(const full_type& assignee, const full_type& assign_value) {
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

[[nodiscard]] std::expected<full_type, std::string> is_valid_binary_op(const full_type& lhs, const full_type& rhs, const std::string& op) {
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

[[nodiscard]] bool operator==(const ref_full_type& lhs, const ref_full_type& rhs) noexcept {
    if (lhs.get() == rhs.get()) {
        return true;
    }

    if (!lhs || !rhs) {
        return false;
    }

    return *lhs == *rhs;
}

[[nodiscard]] bool operator!=(const ref_full_type& lhs, const ref_full_type& rhs) noexcept {
    return !(lhs == rhs);
}

[[nodiscard]] const full_type* get_dominating_type(const full_type& lhs, const full_type& rhs) {
    const auto reason_l_to_r = not_assignable_reason(lhs, rhs);
    const auto reason_r_to_l = not_assignable_reason(rhs, lhs);

    if (!reason_l_to_r && !reason_r_to_l) {
        if (lhs == rhs) {
            return &lhs;
        }
        return nullptr;
    }

    if (!reason_l_to_r) {
        return &lhs;
    }

    if (!reason_r_to_l) {
        return &rhs;
    }

    return nullptr;
}


}