#pragma once

#include "expressions.h"
#include <unordered_map>

namespace dconstruct::dcompiler {

    // the expression frame maps every register to an expression.
    // expressions can be literals, pointers, adds, dereferences, etc...
    // the idea is that a register can hold an expression that we can then expand on. say for example you have

    // move r0, 0
    // move r1, 1
    // add r1, r0, r1 

    // this would look like this in the expression frame
    // <0, num_literal[0]>
    // <1, num_literal[1]>
    // <1, add[num_literal[0], num_literal[1]]

    // we can then lazily evaluate this expression when it makes sense in the pseudocode.
    // of course, not all expressions can be evaluated to a single thing.
    // so you might have if (*x == y)... without being able to evaluate the comparison any further.
    // every expression type implements a way to evaluate it to the "lowest" level.

    struct expression_frame {
        std::unordered_map<u32, std::unique_ptr<expression>> m_expressions;

        void move(const u32 dst, const u32 src) noexcept {
            m_expressions[dst] = m_expressions[src]->eval();
        }
    };
}