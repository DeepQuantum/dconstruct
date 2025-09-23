#pragma once

#include "ast/ast.h"
#include "ast/type.h"
#include "disassembly/instructions.h"
#include "decompilation/control_flow_graph.h"
#include <map>
#include <unordered_map>
#include <type_traits>
#include <vector>
#include <iostream>
#include <stack>


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
        std::vector<expr_uptr> m_transformableExpressions;

        std::vector<ast::variable_declaration> m_arguments;

        compiler::environment m_env;

        ast::block m_baseBlock;

        std::stack<std::reference_wrapper<ast::block>> m_blockStack;

        opt_ref<const std::vector<SymbolTableEntry>> m_symbolTable;

        std::vector<std::string> m_messages;
        u32 m_varCount = 0;
        u32 m_expressionId = 0;

        explicit expression_frame(const std::vector<SymbolTableEntry> &table) : m_symbolTable(table), m_baseBlock{{}} {
            m_blockStack.push(std::ref(m_baseBlock));
            for (u32 i = 0; i < 49; ++i) {
                m_transformableExpressions.push_back(nullptr);
            }
            for (u32 i = 49; i < 128; ++i) {
                m_transformableExpressions.push_back(std::make_unique<ast::identifier>(compiler::token{ compiler::token_type::IDENTIFIER, "arg_" + std::to_string(i - 49)}));
            }
        }

        expression_frame(expression_frame&& rhs) noexcept
            : m_symbolTable(rhs.m_symbolTable),
            m_baseBlock(std::move(rhs.m_baseBlock)),
            m_blockStack{},
            m_transformableExpressions(std::move(rhs.m_transformableExpressions)),
            m_varCount(rhs.m_varCount),
            m_arguments(std::move(rhs.m_arguments))
        {
            m_blockStack.push(std::ref(m_baseBlock));
        }

        inline std::string get_next_var() {
            return "var_" + std::to_string(m_varCount++);
        }

        void append_to_current_block(stmnt_uptr&& statement) {
            m_blockStack.top().get().m_statements.push_back(std::move(statement));
        }

        void load_expression_into_var(const u32 dst, expr_uptr&& expr);


        [[nodiscard]] expr_uptr call(const Instruction& istr);

        [[nodiscard]] expr_uptr cast_to_int(const Instruction& istr);
        [[nodiscard]] expr_uptr cast_to_float(const Instruction& istr);
        
        [[nodiscard]] expr_uptr emit_condition(const control_flow_node& origin);

        ast::return_stmt& insert_return(const u32 dest);

        ast::while_stmt& insert_loop_head(const control_flow_loop& loop, const u32 conditional_check_location);


        [[nodiscard]] inline b8 is_binary(const ast::expression* expr) {
            return dynamic_cast<const ast::binary_expr*>(expr) != nullptr;
        }

        template <typename T>
        [[nodiscard]] inline expr_uptr apply_binary_op(const Instruction& istr, compiler::token op) {
            const auto& op1 = m_transformableExpressions[istr.operand1];
            const auto& op2 = m_transformableExpressions[istr.operand2];
            return std::make_unique<T>(
                std::move(op),
                is_binary(op1.get()) ? std::make_unique<ast::grouping>(op1->clone()) : op1->clone(),
                is_binary(op2.get()) ? std::make_unique<ast::grouping>(op2->clone()) : op2->clone()
            );
        }

        template <typename T>
        [[nodiscard]] inline expr_uptr apply_binary_op_imm(const Instruction& istr, compiler::token op) {
            const auto& op1 = m_transformableExpressions[istr.operand1];
            return std::make_unique<T>(
                std::move(op),
                is_binary(op1.get()) ? std::make_unique<ast::grouping>(op1->clone()) : op1->clone(),
                std::make_unique<ast::literal>(istr.operand2)
            );
        }

        template <typename T>
        [[nodiscard]] inline expr_uptr apply_unary_op(const Instruction& istr, compiler::token op) {
            const auto& op1 = m_transformableExpressions[istr.operand1];
            return std::make_unique<T>(
                std::move(op),
                is_binary(op1.get()) ? std::make_unique<ast::grouping>(op1->clone()) : op1->clone()
            );
        }
    };
}

