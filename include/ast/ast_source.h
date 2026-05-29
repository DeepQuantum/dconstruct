#pragma once

#include "compilation/tokens.h"
#include "compilation/environment.h"
#include <optional>
#include <ostream>

namespace dconstruct::ast {


    enum class FOREACH_OPTIMIZATION_ACTION : u8 {
        BEGIN_FOREACH,
        END_FOREACH,
        ITERABLE_COUNT,
        ITERABLE_AT,
        FOR,
        NONE,
    };

    enum class VAR_OPTIMIZATION_ACTION : u8 {
        VAR_DECLARATION,
        VAR_READ,
        VAR_WRITE,
        VAR_DEREFERENCE,
        NONE
    };

    enum class MATCH_OPTIMIZATION_ACTION : u8 {
        RESULT_VAR_DECLARATION,
        RESULT_VAR_WRITE,
        RESULT_VAR_ASSIGNMENT,
        CHECK_VAR_SET,
        CHECK_VAR_READ,
        MATCH_CONDITION_COMPARISON,
        MATCH_CONDITION_CHECKED_COMPARISON,
        LITERAL,
        NONE,
    };


    struct ast_element {
        virtual ~ast_element() = default;
        virtual void pseudo_c(std::ostream&) const = 0;
        virtual void pseudo_py(std::ostream&) const = 0;
        virtual void pseudo_racket(std::ostream&) const = 0;
        virtual void pseudo_c_for_compiler(std::ostream& os) const;
        virtual bool is_dead_code() const noexcept { return false; }
        [[nodiscard]] virtual std::optional<compilation::source_location> source_location() const noexcept { return std::nullopt; }

        [[nodiscard]] std::string to_c_string() const noexcept {
            std::ostringstream oss;
            pseudo_c(oss);
            return oss.str();
        }

        [[nodiscard]] std::string to_c_for_compiler_string() const noexcept {
            std::ostringstream oss;
            pseudo_c_for_compiler(oss);
            return oss.str();
        }
    };

    enum class LANGUAGE_FLAGS {
        C = 0x1,
        PY = 0x2,
        RACKET = 0x4,
        FUNCTION_NAMES_PASCAL = 0x8,
        IDENTIFIER_PASCAL = 0x10,
        COMPILER = 0x20,
    };

    inline i32 get_flag_index() {
        static i32 index = std::ios_base::xalloc();
        return index;
    }
    
    inline std::ostream& c(std::ostream& os) {
        os.iword(get_flag_index()) |= static_cast<i32>(LANGUAGE_FLAGS::C);
        return os;
    }

    inline std::ostream& py(std::ostream& os) {
        os.iword(get_flag_index()) |= static_cast<i32>(LANGUAGE_FLAGS::PY);
        return os;
    }

    inline std::ostream& racket(std::ostream& os) {
        os.iword(get_flag_index()) |= static_cast<i32>(LANGUAGE_FLAGS::RACKET);
        return os;
    }

    inline std::ostream& func_pascal_case(std::ostream& os) {
        os.iword(get_flag_index()) |= static_cast<i32>(LANGUAGE_FLAGS::FUNCTION_NAMES_PASCAL);
        return os;
    }

    inline std::ostream& id_pascal_case(std::ostream& os) {
        os.iword(get_flag_index()) |= static_cast<i32>(LANGUAGE_FLAGS::IDENTIFIER_PASCAL);
        return os;
    }

    inline std::ostream& remove_id_pascal_case(std::ostream& os) {
        os.iword(get_flag_index()) &= ~static_cast<i32>(LANGUAGE_FLAGS::IDENTIFIER_PASCAL);
        return os;
    }

    inline std::ostream& compiler_syntax(std::ostream& os) {
        os.iword(get_flag_index()) |= static_cast<i32>(LANGUAGE_FLAGS::COMPILER);
        return os;
    }

    inline void ast_element::pseudo_c_for_compiler(std::ostream& os) const {
        const auto old_flags = os.iword(get_flag_index());
        os.iword(get_flag_index()) = old_flags | static_cast<i32>(LANGUAGE_FLAGS::COMPILER);
        pseudo_c(os);
        os.iword(get_flag_index()) = old_flags;
    }

    inline int indent_index() {
        static int idx = std::ios_base::xalloc();
        return idx;
    }

    inline int indent_width_index() {
        static int idx = std::ios_base::xalloc();
        return idx;
    }

    inline int get_indent_width(std::ostream& os) {
        const int width = static_cast<int>(os.iword(indent_width_index()));
        return width > 0 ? width : 4;
    }

    struct set_indent_width {
        int m_width;
    };

    inline std::ostream& operator<<(std::ostream& os, set_indent_width width) {
        os.iword(indent_width_index()) = width.m_width;
        return os;
    }

    inline std::ostream& indent_more(std::ostream& os) {
        ++os.iword(indent_index());
        return os;
    }

    inline std::ostream& indent_less(std::ostream& os) {
        --os.iword(indent_index());
        return os;
    }

    inline std::ostream& indent(std::ostream& os) {
        const int level = static_cast<int>(os.iword(indent_index()));
        const int width = get_indent_width(os);
        for (int i = 0; i < level * width; ++i) {
            os << ' ';
        }
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const ast_element &expr) {
        if (os.iword(get_flag_index()) & static_cast<i32>(LANGUAGE_FLAGS::COMPILER)) {
            expr.pseudo_c_for_compiler(os);
        } else if (os.iword(get_flag_index()) & static_cast<i32>(LANGUAGE_FLAGS::RACKET)) {
            expr.pseudo_racket(os);
        } else if (os.iword(get_flag_index()) & static_cast<i32>(LANGUAGE_FLAGS::PY)) {
            expr.pseudo_py(os);
        } else {
            expr.pseudo_c(os);
        }
        return os;
    }

    using print_fn_type = std::ostream& (*)(std::ostream&);
}
