#pragma once

#include "ast/ast.h"
#include "ast/type.h"
#include "disassembly/instructions.h"
#include <map>
#include <unordered_map>
#include <type_traits>
#include <vector>
#include <iostream>


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

    using register_index = u8;

    struct expression_frame {

        // stores expressions that we are currently processing
        std::map<register_index, std::unique_ptr<ast::expression>> m_transformableExpressions;

        compiler::environment m_env;

        // maps statements to a list of the indexes
        std::vector<std::unique_ptr<ast::statement>> m_statements;

        std::map<u32, SymbolTableEntry> m_symbolTable;
        u32 m_varCount = 0;
        u32 m_expressionId = 0;

        explicit expression_frame(const std::map<u32, SymbolTableEntry> &table) : m_symbolTable(table) {
            for (u32 i = 0; i < 49; ++i) {
                m_transformableExpressions[i] = std::make_unique<ast::literal>(nullptr);
            }
            for (u32 i = 49; i < 128; ++i) {
                m_transformableExpressions[i] = std::make_unique<ast::identifier>("arg_" + std::to_string(i), i);
            }
        }

        expression_frame(expression_frame&& rhs) noexcept = default;

        inline u32 get_next_var_idx() noexcept {
            return m_varCount++;
        }

        void move(const u32, const u32);

        void load_literal(const u8 dst, const ast::primitive_value& value);

        template<ast::requires_binary_expr binary_expr_t>
        inline void apply_binary_op(const Instruction& istr) {
            // auto t1 = m_transformableExpressions[istr.operand1]->get_type(m_env);
            // auto t2 = m_transformableExpressions[istr.operand2]->get_type(m_env);
            // if (!t1.has_value() || !t2.has_value) {
            //     std::cerr << "warning: couldn't determine type of the binary expression " << istr.opcode_to_string() << '\n';
            // }
            // if (t1.value() != t2.value()) {
            //     std::cerr << "warning: types don't match in binary expression " << istr.opcode_to_string() << '\n';
            // }
            m_transformableExpressions[istr.destination] = std::make_unique<ast::grouping>(
                std::make_unique<binary_expr_t>(
                    std::move(m_transformableExpressions[istr.operand1]), 
                    std::move(m_transformableExpressions[istr.operand2])
                )
            );
        }
    };
}