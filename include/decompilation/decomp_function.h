#include "base.h"
#include "ast/type.h"
#include "ast/ast.h"
#include "binaryfile.h"
#include "decompilation/control_flow_graph.h"
#include "ast/ast_source.h"
#include <set>
#include <stack>

constexpr u8 MAX_EXPRESSION_COMPLEXITY = 4;

namespace dconstruct::dcompiler {
    struct decomp_function {
        decomp_function(const function_disassembly& func, const BinaryFile& file, ControlFlowGraph graph, std::optional<std::filesystem::path> graph_path = std::nullopt) noexcept : 
        m_disassembly(func), 
        m_file(file), 
        m_graphPath(graph_path), 
        m_graph(graph), 
        m_parsedNodes(graph.m_nodes.size(), false), 
        m_ipdomsEmitted(m_graph.m_nodes.size(), false),
        m_functionDefinition{} {};

        // struct allocator {

        // };


        // template<node_t, Args... args> requires (std::is_base_of<node_t, ast::ast_element>)
        // [[nodiscard]] expr_uptr create_node(Args ...args)  {
        //     return new (m_arena) node_t(std::forward(args)...);
        // }

        const ast::function_definition& decompile(const bool optimization_passes = false) & ;
        [[nodiscard]] ast::function_definition decompile(const bool optimization_passes = false) && ;

        decomp_function& operator=(decomp_function&&) noexcept = default;

        decomp_function(const decomp_function&) = delete;
        decomp_function& operator=(const decomp_function&) = delete;
        
        ControlFlowGraph m_graph;
        std::unordered_map<reg_idx, std::stack<std::unique_ptr<ast::identifier>>> m_registersToVars;
        std::array<expr_uptr, MAX_REGISTER> m_transformableExpressions;
        std::vector<ast::variable_declaration> m_arguments;
        std::stack<std::reference_wrapper<ast::block>> m_blockStack;
        compilation::scope m_env{{}};
        const function_disassembly& m_disassembly;
        const BinaryFile& m_file;
        std::optional<std::filesystem::path> m_graphPath;
        ast::function_definition m_functionDefinition;
        node_set m_parsedNodes;
        node_set m_ipdomsEmitted;
        char m_loopVar = 'i';
		u16 m_varCount = 0;
        bool m_is64Bit = true;

        void emit_node(const control_flow_node &node, const node_id stop_node);

        void emit_if_else(const control_flow_node &node, const node_id stop_node);

        void emit_if(const control_flow_node& node, const node_id stop_node);
 
        void emit_branch(ast::block& block, node_id proper_destination, const node_id idom, reg_set regs_to_emit, std::unordered_map<reg_idx, dconstruct::ast::full_type> &regs_to_type);

        void emit_loop(const control_flow_loop &loop, const node_id stop_node);

        [[nodiscard]] bool is_for_loop(const control_flow_loop& loop) const noexcept;

        void emit_for_loop(const control_flow_loop &loop, const node_id stop_node);
        void emit_while_loop(const control_flow_loop &loop, const node_id stop_node);

        void parse_basic_block(const control_flow_node& node);

        std::string get_next_var();

        void append_to_current_block(stmnt_uptr&& statement);

        void append_to_current_block(expr_uptr&& expr);

        void load_expression_into_new_var(const reg_idx dst);
        void load_expression_into_existing_var(const reg_idx dst, std::unique_ptr<ast::identifier>&& var);

        void optimize_ast();

        [[nodiscard]] std::unique_ptr<ast::call_expr> make_call(const Instruction& istr);

        template<ast::primitive_kind kind>
        [[nodiscard]] std::unique_ptr<ast::call_expr> make_abs(const reg_idx dst);

        template<typename to>
        [[nodiscard]] expr_uptr make_cast(const Instruction& istr, const ast::full_type& type);

        template<ast::primitive_kind kind>
        [[nodiscard]] expr_uptr make_store(const Instruction& istr);
        
        [[nodiscard]] expr_uptr make_condition(const control_flow_node& origin, node_id& proper_head, node_id& proper_successor, node_id& proper_destination);
        //[[nodiscard]] expr_uptr make_condition(const control_flow_node& origin);

        [[nodiscard]] expr_uptr make_loop_condition(const node_id head_start, const node_id head_end, const node_id loop_entry, const node_id loop_exit);

        [[nodiscard]] expr_uptr get_expression_as_condition(const reg_idx from) const;

        void insert_return(const reg_idx dest);

        void set_binary_types(expr_uptr& lhs, expr_uptr& rhs) const noexcept;

        template <typename T>
        [[nodiscard]] inline std::unique_ptr<T> apply_binary_op(const Instruction& istr) {
            auto& op1 = m_transformableExpressions[istr.operand1];
            auto& op2 = m_transformableExpressions[istr.operand2];
            set_binary_types(op1, op2);
            return std::make_unique<T>(
                op1->get_grouped(),
                op2->get_grouped()
            );
        }

        template <typename T>
        [[nodiscard]] inline std::unique_ptr<T> apply_binary_op(const Instruction& istr, compilation::token op) {
            auto& op1 = m_transformableExpressions[istr.operand1];
            auto& op2 = m_transformableExpressions[istr.operand2];
            set_binary_types(op1, op2);
            auto expr = std::make_unique<T>(
                std::move(op),
                op1->get_grouped(),
                op2->get_grouped()
            );
            if constexpr (std::is_same_v<T, ast::compare_expr>) {
                const bool is_comp = expr->m_operator.m_lexeme == "==" || expr->m_operator.m_lexeme == "!=";
                if (is_comp && expr->m_lhs->as_literal()) {
                    std::swap(expr->m_lhs, expr->m_rhs);
                }
            }
            return expr;
        }

        template <typename T>
        [[nodiscard]] inline std::unique_ptr<T> apply_binary_op_imm(const Instruction& istr) {
            const auto& op1 = m_transformableExpressions[istr.operand1];
            return std::make_unique<T>(
                op1->get_grouped(),
                std::make_unique<ast::literal>(istr.operand2)
            );
        }

        template <typename T>
        [[nodiscard]] inline std::unique_ptr<T> apply_unary_op(const Instruction& istr) {
            const auto& op1 = m_transformableExpressions[istr.operand1];
            return std::make_unique<T>(op1->get_grouped());
        }

        [[nodiscard]] std::unique_ptr<ast::call_expr> make_shift(const Instruction& istr);

        [[nodiscard]] std::unique_ptr<ast::call_expr> make_load_symbol_table(const Instruction& istr);
        
    };

    struct state_script_functions {

        using track = std::vector<const ast::function_definition*>;

        using tracks = std::vector<std::pair<std::string, track>>;

        using blocks = std::vector<std::pair<std::string, tracks>>;

        using states = std::vector<std::pair<std::string, blocks>>;

        states m_states;

        std::vector<const ast::function_definition*> m_nonStateScriptFuncs;

        const BinaryFile* m_binFile = nullptr;

        state_script_functions(const std::vector<ast::function_definition>& funcs, const BinaryFile* binary_file = nullptr) noexcept;

        [[nodiscard]] void to_string(std::ostream& os) const noexcept;

        void emit_script_metadata(std::ostream &os) const;
    };


} 
