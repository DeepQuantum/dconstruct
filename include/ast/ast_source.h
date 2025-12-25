#pragma once

#include "compilation/environment.h"
#include <ostream>

namespace dconstruct::ast {

    struct statement;
    struct expression;

    using ast_index = u64;

    struct variable_folding_context {
        std::unique_ptr<statement>* m_declaration;
        std::vector<std::unique_ptr<expression>*> m_reads;
        std::vector<std::unique_ptr<expression>*> m_writes;
    };

    enum class FOREACH_OPTIMIZATION_ACTION : u8 {
        BEGIN_FOREACH,
        END_FOREACH,
        NONE,
    };

    enum class VAR_OPTIMIZATION_ACTION : u8 {
        VAR_DECLARATION,
        VAR_READ,
        VAR_WRITE,
        NONE
    };

    using var_optimization_env = compiler::environment<variable_folding_context>;
    using foreach_optimization_env = std::unique_ptr<statement>*;
    
    struct ast_element {
        virtual ~ast_element() = default;
        virtual void pseudo_c(std::ostream&) const = 0;
        virtual void pseudo_py(std::ostream&) const = 0;
        virtual void pseudo_racket(std::ostream&) const = 0;
    };

    enum class Flags {
        C = 0x1,
        PY = 0x2,
        RACKET = 0x4,
    };

    inline i32 get_flag_index() {
        static i32 index = std::ios_base::xalloc();
        return index;
    }
    
    inline std::ostream& c(std::ostream& os) {
        os.iword(get_flag_index()) |= static_cast<i32>(Flags::C);
        return os;
    }

    inline std::ostream& py(std::ostream& os) {
        os.iword(get_flag_index()) |= static_cast<i32>(Flags::PY);
        return os;
    }

    inline std::ostream& racket(std::ostream& os) {
        os.iword(get_flag_index()) |= static_cast<i32>(Flags::RACKET);
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
        if (os.iword(get_flag_index()) & static_cast<i32>(Flags::RACKET)) {
            expr.pseudo_racket(os);
        } else if (os.iword(get_flag_index()) & static_cast<i32>(Flags::PY)) {
            expr.pseudo_py(os);
        } else {
            expr.pseudo_c(os);
        }
        return os;
    }

    using print_fn_type = std::ostream& (*)(std::ostream&);
}
