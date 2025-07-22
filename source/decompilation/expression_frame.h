#pragma once

#include "binary_expressions.h"
#include <unordered_map>
#include <type_traits>

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
        std::unordered_map<u32, expression_uptr> m_expressions;
        std::vector<expression_uptr> m_finalized;

        explicit expression_frame() {
            for (u32 i = 0; i < 128; ++i) {
                m_expressions[i] = nullptr;
            }
        }

        void finalize_expression(const u32 dst) {
            m_finalized.push_back(m_expressions[dst]->eval());
            m_expressions[dst] = nullptr;
        }

        template<requires_binary_expr binary_expr_t>
        void apply_binary_op(const Instruction& istr) {
            if (istr.destination != istr.operand1) {
                finalize_expression(istr.destination);
            }
            m_expressions[istr.destination] = std::make_unique<binary_expr_t>(
                std::move(m_expressions[istr.operand1]), 
                std::move(m_expressions[istr.operand2])
            );
        }
    };
}