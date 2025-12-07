#include "base.h"
#include "ast/type.h"
#include "ast/ast.h"
#include "binaryfile.h"
#include "decompilation/control_flow_graph.h"
#include <set>
#include <stack>

constexpr u8 MAX_EXPRESSION_COMPLEXITY = 4;

namespace dconstruct::dcompiler {

    struct decomp_function {
        explicit decomp_function(const function_disassembly &func, const BinaryFile &current_file);

        [[nodiscard]] std::string to_string() const;
        
        ControlFlowGraph m_graph;
        std::unordered_map<reg_idx, std::stack<std::unique_ptr<ast::identifier>>> m_registersToVars;
        std::vector<expr_uptr> m_transformableExpressions;
        std::vector<ast::variable_declaration> m_arguments;
        std::stack<std::reference_wrapper<ast::block>> m_blockStack;
        compiler::environment m_env;
        const function_disassembly& m_disassembly;
        const BinaryFile& m_file;
        ast::full_type m_returnType;
        node_set m_parsedNodes;
        node_set m_ipdomsEmitted;
        ast::block m_baseBlock;
        char m_loopVar = 'i';
		u16 m_varCount = 0;

        void emit_node(const control_flow_node &node, const node_id stop_node);

        void emit_if_else(const control_flow_node &node, const node_id stop_node);

        void emit_if(const control_flow_node& node, const node_id stop_node);
 
        void emit_branch(ast::block& block, node_id proper_destination, const node_id idom, reg_set regs_to_emit, std::unordered_map<reg_idx, dconstruct::ast::full_type> &regs_to_type);

        void emit_loop(const control_flow_loop &loop, const node_id stop_node);

        [[nodiscard]] bool is_for_loop(const control_flow_loop& loop) const noexcept;

        void emit_for_loop(const control_flow_loop &loop, const node_id stop_node);
        void emit_while_loop(const control_flow_loop &loop, const node_id stop_node);

        void parse_basic_block(const control_flow_node& node);

        inline std::string get_next_var() {
            return "var_" + std::to_string(m_varCount++);
        }

        inline void append_to_current_block(stmnt_uptr&& statement) {
            m_blockStack.top().get().m_statements.push_back(std::move(statement));
        }

        inline void append_to_current_block(expr_uptr&& statement) {
            append_to_current_block(std::make_unique<ast::expression_stmt>(std::move(statement)));
        }

        void load_expression_into_new_var(const reg_idx dst);
        void load_expression_into_existing_var(const reg_idx dst, std::unique_ptr<ast::identifier>&& var);

        [[nodiscard]] std::unique_ptr<ast::call_expr> make_call(const Instruction& istr);

        template<ast::primitive_kind kind>
        [[nodiscard]] std::unique_ptr<ast::call_expr> make_abs(const reg_idx dst);

        template<typename from, typename to>
        [[nodiscard]] expr_uptr make_cast(const Instruction& istr, const ast::full_type& type);

        template<ast::primitive_kind kind>
        [[nodiscard]] std::unique_ptr<ast::assign_expr> make_store(const Instruction& istr);
        
        [[nodiscard]] expr_uptr make_condition(const control_flow_node& origin, node_id& proper_head, node_id& proper_successor, node_id& proper_destination);
        //[[nodiscard]] expr_uptr make_condition(const control_flow_node& origin);

        [[nodiscard]] expr_uptr make_loop_condition(const node_id head_start, const node_id head_end, const node_id loop_entry, const node_id loop_exit);

        [[nodiscard]] expr_uptr get_expression_as_condition(const reg_idx from) const;

        void insert_return(const reg_idx dest);

        [[nodiscard]] inline bool is_binary(const ast::expression* expr) {
            return dynamic_cast<const ast::binary_expr*>(expr) != nullptr;
        }

        template <typename T>
        [[nodiscard]] inline std::unique_ptr<T> apply_binary_op(const Instruction& istr) {
            const auto& op1 = m_transformableExpressions[istr.operand1];
            const auto& op2 = m_transformableExpressions[istr.operand2];
            return std::make_unique<T>(
                is_binary(op1.get()) ? std::make_unique<ast::grouping>(op1->clone()) : op1->clone(),
                is_binary(op2.get()) ? std::make_unique<ast::grouping>(op2->clone()) : op2->clone()
            );
        }

        template <typename T>
        [[nodiscard]] inline std::unique_ptr<T> apply_binary_op(const Instruction& istr, compiler::token op) {
            const auto& op1 = m_transformableExpressions[istr.operand1];
            const auto& op2 = m_transformableExpressions[istr.operand2];
            auto expr = std::make_unique<T>(
                std::move(op),
                is_binary(op1.get()) ? std::make_unique<ast::grouping>(op1->clone()) : op1->clone(),
                is_binary(op2.get()) ? std::make_unique<ast::grouping>(op2->clone()) : op2->clone()
            );
            if constexpr (std::is_same_v<T, ast::compare_expr>) {
                const bool is_comp = expr->m_operator.m_lexeme == "==" || expr->m_operator.m_lexeme == "!=";
                if (is_comp && dynamic_cast<ast::literal*>(expr->m_lhs.get()) != nullptr) {
                    std::swap(expr->m_lhs, expr->m_rhs);
                }
            }
            return expr;
        }

        template <typename T>
        [[nodiscard]] inline std::unique_ptr<T> apply_binary_op_imm(const Instruction& istr) {
            const auto& op1 = m_transformableExpressions[istr.operand1];
            return std::make_unique<T>(
                is_binary(op1.get()) ? std::make_unique<ast::grouping>(op1->clone()) : op1->clone(),
                std::make_unique<ast::literal>(istr.operand2)
            );
        }

        template <typename T>
        [[nodiscard]] inline std::unique_ptr<T> apply_unary_op(const Instruction& istr) {
            const auto& op1 = m_transformableExpressions[istr.operand1];
            return std::make_unique<T>(is_binary(op1.get()) ? std::make_unique<ast::grouping>(op1->clone()) : op1->clone());
        }

        [[nodiscard]] inline std::unique_ptr<ast::call_expr> make_shift(const Instruction& istr) {
            std::vector<expr_uptr> arg;
            arg.push_back(m_transformableExpressions[istr.destination]->clone());
            auto callee = std::make_unique<ast::identifier>("int_ash");
            auto call = std::make_unique<ast::call_expr>(compiler::token{ compiler::token_type::_EOF, "" }, std::move(callee), std::move(arg));
            call->set_type(make_type(ast::primitive_kind::U64));
            return call;
        }

        [[nodiscard]] inline std::unique_ptr<ast::call_expr> make_load_symbol_table(const Instruction& istr) {
            std::vector<expr_uptr> arg;
            arg.push_back(m_transformableExpressions[istr.destination]->clone());
            auto callee = std::make_unique<ast::identifier>("symbol_table_load");
            auto call = std::make_unique<ast::call_expr>(compiler::token{ compiler::token_type::_EOF, "" }, std::move(callee), std::move(arg));
            return call;
        }
    };
} 
