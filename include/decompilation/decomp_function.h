#include "base.h"
#include "ast/type.h"
#include "ast/ast.h"
#include "binaryfile.h"
#include "decompilation/control_flow_graph.h"
#include <set>
#include <stack>

constexpr u8 max_expression_complexity = 4;

namespace dconstruct::dcompiler {
    struct decomp_function {
        explicit decomp_function(const function_disassembly *func, const BinaryFile &current_file);

        [[nodiscard]] std::string to_string() const;

        const function_disassembly* m_disassembly;
        const BinaryFile& m_file;
        ast::full_type m_returnType;
        std::set<node_id> m_parsedNodes;
        ControlFlowGraph m_graph;
        std::vector<expr_uptr> m_transformableExpressions;
        std::vector<ast::variable_declaration> m_arguments;
        compiler::environment m_env;
        ast::block m_baseBlock;
        std::stack<std::reference_wrapper<ast::block>> m_blockStack;
        std::unordered_map<reg_idx, std::stack<std::unique_ptr<ast::identifier>>> m_registersToVars;
        const SymbolTable& m_symbolTable;
        u32 m_varCount = 0;

        void emit_node(const control_flow_node &node, node_id stop_node);

        void emit_loop(const function_disassembly_line &last_line, const control_flow_loop &loop);

        void emit_branch(const control_flow_node &node, node_id stop_node);

        void emit_one_branch(std::unique_ptr<ast::block>& block, const reg_idx proper_destination, const node_id idom, const std::set<reg_idx>& regs_to_emit, std::unordered_map<reg_idx, ast::full_type>& regs_to_type);

        void parse_basic_block(const control_flow_node& node);

        inline std::string get_next_var() {
            return "var_" + std::to_string(m_varCount++);
        }

        void append_to_current_block(stmnt_uptr&& statement) {
            m_blockStack.top().get().m_statements.push_back(std::move(statement));
        }

        void load_expression_into_new_var(const reg_idx dst, expr_uptr&& expr);
        void load_expression_into_existing_var(const reg_idx dst, std::unique_ptr<ast::identifier>&& var, expr_uptr&& expr);

        [[nodiscard]] expr_uptr make_call(const Instruction& istr);

        [[nodiscard]] expr_uptr make_cast_to_int(const Instruction& istr);
        [[nodiscard]] expr_uptr make_cast_to_float(const Instruction& istr);
        
        [[nodiscard]] expr_uptr make_condition(const control_flow_node& origin, node_id& proper_head, node_id& proper_successor, node_id& proper_destination);

        [[nodiscard]] expr_uptr make_dereference(const Instruction& istr);

        void insert_return(const reg_idx dest);

        void insert_loop_head(const control_flow_loop& loop, const reg_idx conditional_check_location);

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
