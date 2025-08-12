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
        std::vector<std::unique_ptr<ast::expression>> m_transformableExpressions;

        compiler::environment m_env;

        // maps statements to a list of the indexes
        std::vector<std::unique_ptr<ast::statement>> m_statements;

        std::map<u32, SymbolTableEntry> m_symbolTable;
        u32 m_varCount = 0;
        u32 m_expressionId = 0;

        explicit expression_frame(const std::map<u32, SymbolTableEntry> &table) : m_symbolTable(table) {
            for (u32 i = 0; i < 49; ++i) {
                m_transformableExpressions.push_back(std::make_unique<ast::identifier>(0));
            }
            for (u32 i = 49; i < 128; ++i) {
                m_transformableExpressions.push_back(std::make_unique<ast::identifier>("arg_", i - 49));
            }
        }

        expression_frame(expression_frame&& rhs) noexcept = default;

        inline std::string get_next_var() {
            return "var_" + std::to_string(m_varCount++);
        }

        void move(const u32, const u32);

        void load_literal(const u8 dst, const ast::primitive_value& value);

        template<ast::requires_binary_expr binary_expr_t>
        inline void apply_binary_op(const Instruction& istr) {
            m_transformableExpressions[istr.destination] = std::make_unique<ast::grouping>(
                std::make_unique<binary_expr_t>(
                    std::move(m_transformableExpressions[istr.operand1]),
                    std::move(m_transformableExpressions[istr.operand2])
                )
            );
        }
    };
}