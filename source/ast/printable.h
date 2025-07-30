#pragma once

#include "base.h"
#include <ostream>

namespace dconstruct {
    struct Iprintable {
        virtual void pseudo(std::ostream&) const = 0;
        virtual void ast(std::ostream&) const = 0;
    };

    enum Flags {
        AST = 0x1,
    };

    inline i32 get_flag_index() {
        static i32 index = std::ios_base::xalloc();
        return index;
    }
    
    inline std::ostream& set_ast(std::ostream& os) {
        os.iword(get_flag_index()) |= AST;
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const Iprintable &expr) {
        if (os.iword(get_flag_index()) & AST) {
            expr.ast(os);
        } else {
            expr.pseudo(os);
        }
        return os;
    }
}
