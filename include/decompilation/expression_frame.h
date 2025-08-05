#pragma once

#include "ast/ast.h"
#include "ast/type.h"
#include "disassembly/instructions.h"
#include <map>
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

    

    struct typed_expression {
        typed_expression() : m_expr(nullptr), m_type(ast::type_kind::UNKNOWN) {};
        typed_expression(std::unique_ptr<ast::expression> expr) : m_expr(std::move(expr)), m_type(ast::type_kind::UNKNOWN) {};
        typed_expression(std::unique_ptr<ast::expression> expr, ast::type_kind type) : m_expr(std::move(expr)), m_type(type) {};

        std::unique_ptr<ast::expression> m_expr;
        ast::type_kind m_type;
    };

    struct expression_frame {
        std::map<u32, typed_expression> m_transformableExpressions;
        std::vector<std::unique_ptr<ast::statement>> m_statements;
        std::map<u32, SymbolTableEntry> m_symbolTable;
        u32 m_varCount = 0;

        explicit expression_frame(const std::map<u32, SymbolTableEntry> &table) : m_symbolTable(table) {
            for (u32 i = 0; i < 49; ++i) {
                m_transformableExpressions[i] = typed_expression();
            }
            for (u32 i = 49; i < 128; ++i) {
                // std::unique_ptr<ast::expression_stmt> temp = std::make_unique<ast::expression_stmt>(
                    
                // );
                m_transformableExpressions[i] = { std::make_unique<ast::identifier>("arg_" + std::to_string(i), i), ast::type_kind::UNKNOWN };
                //m_statements.emplace_back(std::move(temp));
            }
        }

        expression_frame(expression_frame&& rhs) noexcept = default;

        u32 get_next_var_idx() noexcept {
            return m_varCount++;
        }

        void move(const u32, const u32);

        void load_literal(const u8 dst, const ast::primitive_value_type& value);

        template<ast::requires_binary_expr binary_expr_t>
        inline void apply_binary_op(const Instruction& istr) {
            if (m_transformableExpressions[istr.operand1].m_type != m_transformableExpressions[istr.operand2].m_type) {
                std::cerr << "warning: types don't match for operation on instruction" << istr.opcode_to_string() << '\n';
            }
            m_transformableExpressions[istr.destination] = { std::make_unique<ast::grouping>(
                std::make_unique<binary_expr_t>(
                    std::move(m_transformableExpressions[istr.operand1].m_expr), 
                    std::move(m_transformableExpressions[istr.operand2].m_expr)
                )
            ), m_transformableExpressions[istr.operand1].m_type };
        }
    };
}