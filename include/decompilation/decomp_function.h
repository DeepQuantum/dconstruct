#pragma once

#include "base.h"
#include "ast/type.h"
#include "ast/ast.h"
#include "binaryfile.h"
#include "decompilation/control_flow_graph.h"
#include "ast/ast_source.h"
#include "compilation/environment.h"
#include <set>
#include <stack>

constexpr u8 MAX_EXPRESSION_COMPLEXITY = 4;

namespace dconstruct::dcompiler {

    static const auto and_token = compilation::token{compilation::token_type::AMPERSAND_AMPERSAND, "&&"};
    static const auto or_token = compilation::token{compilation::token_type::PIPE_PIPE, "||"};

    static const auto error_replacement_expr = std::make_unique<ast::identifier>("EXPRESSION_ERROR_PLACEHOLDER");

    enum class OPTIMIZATION_KIND : u16 {
        NONE = 0,
        SSO_VAR = 1 << 0,
        FOREACH = 1 << 1,
        MATCH = 1 << 2,
        SECOND_VAR = 1 << 3,
        MEMBER_ACCESS = 1 << 4,
        REGEX = 1 << 5,
        AST = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5),
        ALL = AST,
    };

    [[nodiscard]] constexpr OPTIMIZATION_KIND operator|(const OPTIMIZATION_KIND lhs, const OPTIMIZATION_KIND rhs) noexcept {
        return static_cast<OPTIMIZATION_KIND>(static_cast<u16>(lhs) | static_cast<u16>(rhs));
    }

    [[nodiscard]] constexpr OPTIMIZATION_KIND operator&(const OPTIMIZATION_KIND lhs, const OPTIMIZATION_KIND rhs) noexcept {
        return static_cast<OPTIMIZATION_KIND>(static_cast<u16>(lhs) & static_cast<u16>(rhs));
    }

    [[nodiscard]] constexpr bool has_optimization(const OPTIMIZATION_KIND optimizations, const OPTIMIZATION_KIND kind) noexcept {
        return static_cast<u16>(optimizations & kind) != 0;
    }

    struct decomp_function {
        static constexpr u8 MIN_GRAPH_SIZE = 0;

        decomp_function(
            const function_disassembly& func,
            const BinaryFile& file,
            const SIDBase& sidbase,
            ControlFlowGraph graph,
            std::optional<std::filesystem::path> graph_path = std::nullopt,
            const std::unordered_map<sid64, ast::full_type>* known_types = nullptr,
            const ast::mapped_var_scope* function_scope = nullptr
        ) noexcept :
            m_disassembly(func),
            m_file(file),
            m_sidbase(sidbase),
            m_graphPath(graph_path),
            m_graph(graph),
            m_parsedNodes(graph.m_nodes.size(), false),
            m_ipdomsEmitted(m_graph.m_nodes.size(), false),
            m_knownTypes(known_types),
            m_functionScope(function_scope) {};

        [[nodiscard]] ast::function_definition decompile(const OPTIMIZATION_KIND optimizations = OPTIMIZATION_KIND::NONE);

        [[nodiscard]] const std::pair<ast::full_type, std::optional<std::string>>* find_in_scope();

        decomp_function& operator=(decomp_function&&) noexcept = default;

        decomp_function(const decomp_function&) = delete;
        decomp_function& operator=(const decomp_function&) = delete;

        void emit_node(const control_flow_node& node, const node_id stop_node);

        void emit_if_else(const control_flow_node& node, const node_id stop_node);

        void emit_if(const control_flow_node& node, const node_id stop_node);

        void emit_branch(ast::block& block, node_id proper_destination, const node_id idom, reg_set regs_to_emit, std::unordered_map<reg_idx, dconstruct::ast::full_type>& regs_to_type);

        void emit_loop(const control_flow_loop& loop, const node_id stop_node);

        [[nodiscard]] bool is_for_loop(const control_flow_loop& loop) const noexcept;

        void emit_for_loop(const control_flow_loop& loop, const node_id stop_node);
        void emit_while_loop(const control_flow_loop& loop, const node_id stop_node);

        void parse_basic_block(const control_flow_node& node);

        std::string get_next_var();

        void append_to_current_block(stmnt_uptr&& statement);

        void append_to_current_block(expr_uptr&& expr);

        void load_expression_into_new_var(const reg_idx dst);
        void load_expression_into_existing_var(const reg_idx dst, std::unique_ptr<ast::identifier>&& var);

        void optimize_ast(ast::function_definition& func, OPTIMIZATION_KIND optimizations);

        [[nodiscard]] std::unique_ptr<ast::call_expr> make_call(const Instruction& istr);

        template <ast::primitive_kind kind>
        [[nodiscard]] std::unique_ptr<ast::call_expr> make_abs(const reg_idx dst);

        template <typename to>
        [[nodiscard]] expr_uptr make_cast(const Instruction& istr, const ast::full_type& type);

        template <ast::primitive_kind kind>
        [[nodiscard]] expr_uptr make_store(const Instruction& istr);

        [[nodiscard]] expr_uptr make_condition(const control_flow_node& origin, node_id& proper_head, node_id& proper_successor, node_id& proper_destination);
        //[[nodiscard]] expr_uptr make_condition(const control_flow_node& origin);

        [[nodiscard]] expr_uptr make_loop_condition(const node_id head_start, const node_id head_end, const node_id loop_entry, const node_id loop_exit);

        [[nodiscard]] expr_uptr get_expression_as_condition(const reg_idx from);

        [[nodiscard]] std::unique_ptr<ast::identifier> make_current_var(const ast::full_type& type, expr_uptr expr = nullptr);

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

        [[nodiscard]] std::unique_ptr<ast::call_expr> make_in_range(const Instruction& istr);

        std::array<expr_uptr, MAX_REGISTER> m_transformableExpressions;
        compilation::scope m_env{{}};
        std::unordered_map<reg_idx, std::stack<std::unique_ptr<ast::identifier>>> m_registersToVars;
        ControlFlowGraph m_graph;
        std::optional<std::filesystem::path> m_graphPath;
        std::optional<std::string> m_error;
        node_set m_parsedNodes;
        node_set m_ipdomsEmitted;
        std::stack<std::reference_wrapper<ast::block>> m_blockStack;
        std::vector<ast::variable_declaration> m_arguments;
        const std::unordered_map<sid64, ast::full_type>* m_knownTypes;
        std::unique_ptr<ast::function_definition> m_functionDefinition;
        const ast::mapped_var_scope* m_functionScope;
        const function_disassembly& m_disassembly;
        const BinaryFile& m_file;
        const SIDBase& m_sidbase;
        u16 m_varCount = 0;
        char m_loopVar = 'i';
        bool m_is64Bit = true;
    };

    struct state_script_functions {
        using track = std::vector<const ast::function_definition*>;

        using tracks = std::vector<std::pair<std::string, track>>;

        using blocks = std::vector<std::pair<std::string, tracks>>;

        using states = std::vector<std::pair<std::string, blocks>>;

        state_script_functions(const std::vector<ast::function_definition>& funcs, const BinaryFile* binary_file = nullptr, const SIDBase* sidbase = nullptr) noexcept;

        [[nodiscard]] void to_string(ast::ast_serialization_buffer& buffer) const noexcept;

        void emit_script_metadata(ast::ast_serialization_buffer& buffer) const;

        std::vector<const ast::function_definition*> m_nonStateScriptFuncs;
        states m_states;
        const BinaryFile* m_binFile = nullptr;
        const SIDBase* m_sidbase = nullptr;
    };

}
