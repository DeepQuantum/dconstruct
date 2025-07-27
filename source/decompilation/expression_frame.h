#pragma once

#include "binary_expressions.h"
#include "literal.h"
#include "statements.h"
#include "instructions.h"
#include <map>
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
        std::map<u32, std::unique_ptr<ast::expression>> m_expressions;
        std::vector<std::unique_ptr<ast::statement>> m_statements;
        std::map<u32, SymbolTableEntry> m_symbolTable;
        u32 m_varCount = 0;

        explicit expression_frame(const std::map<u32, SymbolTableEntry> &table) : m_symbolTable(table) {
            for (u32 i = 0; i < 49; ++i) {
                m_expressions[i] = nullptr;
            }
            for (u32 i = 49; i < 128; ++i) {
                m_expressions[i] = std::make_unique<ast::identifier>("arg_" + std::to_string(i), i - 49);
            }
        }

        expression_frame(expression_frame&& rhs) noexcept = default;

        u32 get_next_var_idx() {
            return m_varCount++;
        }

        void move(const u32, const u32) noexcept;


        template<typename T>
        inline void load_literal(const u32 dst, const T value) noexcept {
            std::unique_ptr<ast::literal<T>> literal = std::make_unique<ast::literal<T>>(value);
            std::unique_ptr<ast::identifier> id = std::make_unique<ast::identifier>(get_next_var_idx());
            std::unique_ptr<ast::assign_expr> assign = std::make_unique<ast::assign_expr>(std::move(id), std::move(literal));
            m_statements.emplace_back(std::make_unique<ast::assign_statement>(assign.get()));
            m_expressions[dst] = std::move(assign);
        }

        template<ast::requires_binary_expr binary_expr_t>
        inline void apply_binary_op(const Instruction& istr) noexcept {
            m_expressions[istr.destination] = std::make_unique<ast::grouping>(
                std::move(std::make_unique<binary_expr_t>(
                    std::move(m_expressions[istr.operand1]), 
                    std::move(m_expressions[istr.operand2])
                )
            ));
        }
    };
}