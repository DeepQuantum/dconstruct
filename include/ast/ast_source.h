#pragma once

#include "compilation/environment.h"
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
        virtual bool is_dead_code() const noexcept { return false; }

        [[nodiscard]] std::string to_c_string() const noexcept {
            std::ostringstream oss;
            pseudo_c(oss);
            return oss.str();
        }
    };

    enum class LANGUAGE_FLAGS {
        C = 0x1,
        PY = 0x2,
        RACKET = 0x4,
        FUNCTION_NAMES_PASCAL = 0x8,
        IDENTIFIER_PASCAL = 0x10,
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

    inline int indent_index() {
        static int idx = std::ios_base::xalloc();
        return idx;
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
        int level = os.iword(indent_index());
        for (int i = 0; i < level; ++i) {
            os << "    ";
        }
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const ast_element &expr) {
        if (os.iword(get_flag_index()) & static_cast<i32>(LANGUAGE_FLAGS::RACKET)) {
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
