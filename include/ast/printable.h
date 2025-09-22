#pragma once

#include "base.h"
#include <ostream>

namespace dconstruct {
    struct Iprintable {
        virtual ~Iprintable() = default;
        virtual void pseudo(std::ostream&) const = 0;
        virtual void ast(std::ostream&) const = 0;
    };

    enum class Flags {
        AST = 0x1,
    };

    inline i32 get_flag_index() {
        static i32 index = std::ios_base::xalloc();
        return index;
    }
    
    inline std::ostream& set_ast(std::ostream& os) {
        os.iword(get_flag_index()) |= static_cast<i32>(Flags::AST);
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

    inline std::ostream& operator<<(std::ostream& os, const Iprintable &expr) {
        if (os.iword(get_flag_index()) & static_cast<i32>(Flags::AST)) {
            expr.ast(os);
        } else {
            expr.pseudo(os);
        }
        return os;
    }
}
