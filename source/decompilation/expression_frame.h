#pragma once

#include "binary_expressions.h"
#include "literal.h"
#include "statements.h"
#include "assign_statement.h"
#include "instructions.h"
#include "type.h"
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
        typed_expression() : m_expr(nullptr), m_type(ast::E_UNKNOWN) {};
        typed_expression(std::unique_ptr<ast::expression> expr) : m_expr(std::move(expr)), m_type(ast::E_UNKNOWN) {};
        typed_expression(std::unique_ptr<ast::expression> expr, ast::type type) : m_expr(std::move(expr)), m_type(type) {};

        std::unique_ptr<ast::expression> m_expr;
        ast::type m_type;
    };

    struct expression_frame {
        std::map<u32, typed_expression> m_typedExpressions;
        std::vector<std::unique_ptr<ast::statement>> m_statements;
        std::map<u32, SymbolTableEntry> m_symbolTable;
        u32 m_varCount = 0;

        explicit expression_frame(const std::map<u32, SymbolTableEntry> &table) : m_symbolTable(table) {
            for (u32 i = 0; i < 49; ++i) {
                m_typedExpressions[i] = typed_expression();
            }
            for (u32 i = 49; i < 128; ++i) {
                m_typedExpressions[i] = { std::make_unique<ast::identifier>("arg_" + std::to_string(i), i - 49) };
            }
        }

        expression_frame(expression_frame&& rhs) noexcept = default;

        u32 get_next_var_idx() noexcept {
            return m_varCount++;
        }

        void move(const u32, const u32);


        template<typename T>
        inline void load_literal(const u32 dst, const ast::type type, const T value) {
            std::unique_ptr<ast::literal<T>> literal = std::make_unique<ast::literal<T>>(value);
            std::unique_ptr<ast::identifier> id = std::make_unique<ast::identifier>(get_next_var_idx());
            std::unique_ptr<ast::assign_expr> assign = std::make_unique<ast::assign_expr>(std::move(id), std::move(literal));
            m_statements.emplace_back(std::make_unique<ast::assign_statement>(assign.get()));
            m_typedExpressions[dst] = { std::move(assign), type };
        }

        template<ast::requires_binary_expr binary_expr_t>
        inline void apply_binary_op(const Instruction& istr) {
            if (m_typedExpressions[istr.operand1].m_type != m_typedExpressions[istr.operand2].m_type) {
                std::cerr << "warning: types don't match for operation on instruction" << istr.opcode_to_string() << '\n';
            }
            m_typedExpressions[istr.destination] = { std::make_unique<ast::grouping>(
                std::move(std::make_unique<binary_expr_t>(
                    std::move(m_typedExpressions[istr.operand1].m_expr), 
                    std::move(m_typedExpressions[istr.operand2].m_expr)
                )
            )), m_typedExpressions[istr.operand1].m_type };
        }
    };
}