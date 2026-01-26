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
    template<bool is_64_bit = true>
    struct decomp_function {
        decomp_function(const function_disassembly& func, const BinaryFile<is_64_bit>& file, ControlFlowGraph graph, std::optional<std::filesystem::path> graph_path = std::nullopt) noexcept : 
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
        compiler::scope m_env{{}};
        const function_disassembly& m_disassembly;
        const BinaryFile<is_64_bit>& m_file;
        std::optional<std::filesystem::path> m_graphPath;
        ast::function_definition m_functionDefinition;
        node_set m_parsedNodes;
        node_set m_ipdomsEmitted;
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

        inline void append_to_current_block(expr_uptr&& expr) {
            append_to_current_block(std::make_unique<ast::expression_stmt>(std::move(expr)));
        }

        void load_expression_into_new_var(const reg_idx dst);
        void load_expression_into_existing_var(const reg_idx dst, std::unique_ptr<ast::identifier>&& var);

        void optimize_ast();

        [[nodiscard]] std::unique_ptr<ast::call_expr> make_call(const Instruction& istr);

        template<ast::primitive_kind kind>
        [[nodiscard]] std::unique_ptr<ast::call_expr> make_abs(const reg_idx dst);

        template<typename to>
        [[nodiscard]] expr_uptr make_cast(const Instruction& istr, const ast::full_type& type);

        template<ast::primitive_kind kind>
        [[nodiscard]] std::unique_ptr<ast::assign_expr> make_store(const Instruction& istr);
        
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
        [[nodiscard]] inline std::unique_ptr<T> apply_binary_op(const Instruction& istr, compiler::token op) {
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

        [[nodiscard]] inline std::unique_ptr<ast::call_expr> make_shift(const Instruction& istr) {
            std::vector<expr_uptr> args;
            args.push_back(m_transformableExpressions[istr.operand1]->clone());
            args.push_back(m_transformableExpressions[istr.operand2]->clone());
            auto callee = std::make_unique<ast::literal>(sid64_literal{SID("int_ash"), "int_ash"});
            auto call = std::make_unique<ast::call_expr>(compiler::token{ compiler::token_type::_EOF, "" }, std::move(callee), std::move(args));
            call->set_type(make_type_from_prim(ast::primitive_kind::U64));
            return call;
        }

        [[nodiscard]] inline std::unique_ptr<ast::call_expr> make_load_symbol_table(const Instruction& istr) {
            std::vector<expr_uptr> arg;
            arg.push_back(m_transformableExpressions[istr.destination]->clone());
            auto callee = std::make_unique<ast::literal>(sid64_literal{SID("symbol_table_load"), "symbol_table_load"});
            auto call = std::make_unique<ast::call_expr>(compiler::token{ compiler::token_type::_EOF, "" }, std::move(callee), std::move(arg));
            return call;
        }
        
    };

    template<bool is_64_bit = true>
    struct state_script_functions {

        using track = std::vector<const ast::function_definition*>;

        using tracks = std::vector<std::pair<std::string, track>>;

        using blocks = std::vector<std::pair<std::string, tracks>>;

        using states = std::vector<std::pair<std::string, blocks>>;

        states m_states;

        std::vector<const ast::function_definition*> m_nonStateScriptFuncs;

        const BinaryFile<is_64_bit>* m_binFile = nullptr;

        state_script_functions(const std::vector<ast::function_definition>& funcs, const BinaryFile<is_64_bit>* binary_file = nullptr) noexcept : m_binFile(binary_file) {
            for (const auto& func : funcs) {
                if (std::holds_alternative<std::string>(func.m_name)) {
                    m_nonStateScriptFuncs.push_back(&func);
                }
                else {
                    const state_script_function_id& id = std::get<state_script_function_id>(func.m_name);
                    auto& states = m_states;
                    const u32 state_idx = id.m_state.m_idx;
                    const u32 event_idx = id.m_event.m_idx;
                    const u32 track_idx = id.m_track.m_idx;

                    if (states.size() <= state_idx) {
                        states.resize(state_idx + 1);
                        states[state_idx].first = id.m_state.m_name;
                    }

                    auto& state_entry = states[state_idx].second;

                    if (state_entry.size() <= event_idx) {
                        state_entry.resize(event_idx + 1);
                        state_entry[event_idx].first = id.m_event.m_name;
                    }

                    auto& event_entry = state_entry[event_idx].second;

                    if (event_entry.size() <= track_idx) {
                        event_entry.resize(track_idx + 1);
                        event_entry[track_idx].first = id.m_track.m_name;
                    }

                    event_entry[track_idx].second.push_back(&func);
                }
            }
        }

        [[nodiscard]] void to_string(std::ostream& os) const noexcept {
            for (const auto& func : m_nonStateScriptFuncs) {
                os << *func << "\n\n"; 
            }

            if (m_states.empty()) {
                return;
            }

            assert(m_binFile->m_dcscript != nullptr);

            os << "statescript {\n" << ast::indent_more;

            emit_script_metadata(os);

            for (const auto& [state_name, blocks] : m_states) {
                os << ast::indent <<  "state " << state_name << " {\n";
                os << ast::indent_more;
                for (const auto& [block_name, tracks] : blocks) {
                    os << ast::indent << "block " << block_name << " {\n";
                    os << ast::indent_more;
                    for (const auto& [track_name, functions] : tracks) {
                        os << ast::indent << "track " << track_name << " {\n";
                        os << ast::indent_more;
                        for (const auto* function : functions) {
                            os << ast::indent << "lambda " << *function;
                            os << "\n";
                        }
                        os << ast::indent_less;
                        os << ast::indent << "}\n";
                    }
                    os << ast::indent_less;
                    os << ast::indent << "}\n";
                }
                os << ast::indent_less;
                os << ast::indent << "}\n";
            }

            os << "}\n" << ast::indent_less;
        }


        void emit_script_metadata(std::ostream &os) const
        {
            if (m_binFile->m_dcscript->m_pSsOptions && m_binFile->m_dcscript->m_pSsOptions->m_pSymbolArray) {
                const SymbolArray *array = m_binFile->m_dcscript->m_pSsOptions->m_pSymbolArray;
                os << ast::indent << "options {\n" << ast::indent_more;
                for (i32 i = 0; i < array->m_numEntries; ++i){
                    os << ast::indent << m_binFile->m_sidCache.at(array->m_pSymbols[i]) << "\n";
                }
                os << ast::indent_less << ast::indent << "}\n";
            }
            
            os << std::fixed << std::setprecision(2);

            if (m_binFile->m_dcscript->m_pSsDeclList) {
                os << ast::indent << "declarations {\n" << ast::indent_more;
                for (i32 i = 0; i < m_binFile->m_dcscript->m_pSsDeclList->m_numDeclarations; ++i) {
                    const SsDeclaration *decl = m_binFile->m_dcscript->m_pSsDeclList->m_pDeclarations + i;
                    const bool is_nullptr = decl->m_pDeclValue == nullptr;
                    if (decl->m_isVar) {
                        os << ast::indent;
                        std::string decl_name = m_binFile->m_sidCache.at(decl->m_declId);
                        switch (decl->m_declTypeId) {
                            case SID("boolean"): {
                                os << "bool " << decl_name << " = ";
                                if (!is_nullptr) {
                                    os << std::boolalpha << *reinterpret_cast<const bool*>(decl->m_pDeclValue);
                                } else {
                                    os << "nullptr";
                                }
                                break;
                            }
                            case SID("vector"): {
                                const f32* val = reinterpret_cast<const f32*>(decl->m_pDeclValue);  
                                os << "vector " << decl_name << " = " ;
                                if (!is_nullptr)
                                os << "(" 
                                    << val[0] << ", "
                                    << val[1] << ", "
                                    << val[2] << ", "
                                    << val[3] << ")";
                                else {
                                    os << "nullptr";
                                }
                                break;
                            }
                            case SID("quat"): {
                                const f32* val = reinterpret_cast<const f32*>(decl->m_pDeclValue);  
                                os << "quaternion " << decl_name << " = ";
                                if (!is_nullptr) {
                                os
                                    << "(" << val[0] << ", " 
                                    << val[1] << ", "
                                    << val[2] << ", "
                                    << val[3] << ")";
                                } else {
                                    os << "nullptr";
                                }
                                break;
                            }
                            case SID("float"): {
                                os << "f32 " << decl_name << " = ";
                                if (!is_nullptr) {
                                    os << *reinterpret_cast<const f32*>(decl->m_pDeclValue);
                                } else {
                                    os << "nullptr";
                                }
                                break;
                            }
                            case SID("string"): {
                                os << "string " << decl_name << " = ";
                                if (!is_nullptr) {
                                    os << *reinterpret_cast<const char**>(decl->m_pDeclValue);
                                } else {
                                    os << "nullptr";
                                }
                                break;
                            }
                            case SID("symbol"): {
                                os << "symbol " << decl_name << " = ";
                                if (!is_nullptr) {
                                    os << m_binFile->m_sidCache.at(*reinterpret_cast<const sid64*>(decl->m_pDeclValue));
                                } else {
                                    os << "nullptr";
                                }
                                break;
                            }
                            case SID("int32"): {
                                os << "i32 " << decl_name << " = ";
                                if (!is_nullptr) {
                                    os << *reinterpret_cast<const i32*>(decl->m_pDeclValue);
                                } else {
                                    os << "nullptr";
                                }
                                break;
                            }
                            case SID("uint64"): {
                                os << "u64 " << decl_name << " = ";
                                if (!is_nullptr) {
                                    os << *reinterpret_cast<const u64*>(decl->m_pDeclValue);
                                } else {
                                    os << "nullptr";
                                }
                                break;
                            }
                            case SID("timer"): {
                                os << "timer " << decl_name << " = ";
                                if (!is_nullptr) {
                                    os << *reinterpret_cast<const f32*>(decl->m_pDeclValue);
                                } else {
                                    os << "nullptr";
                                }
                                break;
                            }
                            case SID("point"): {
                                const f32* val = reinterpret_cast<const f32*>(decl->m_pDeclValue);
                                os << "point " << decl_name << " = "; 
                                if (!is_nullptr) {
                                    os << "(" << val[0] << ", "
                                       << "(" << val[1] << ", "
                                       << "(" << val[2] << ")";
                                } else {
                                    os << "nullptr";
                                }
                                break;
                            }
                            case SID("bound-frame"): {
                                os << "bound-frame " << decl_name << " = ";
                                if (!is_nullptr) {
                                    os << *reinterpret_cast<const f32*>(decl->m_pDeclValue);
                                } else {
                                    os << "nullptr";
                                }
                                break;
                            }
                            default: {
                                os << "u64? " << decl_name << " = ???";
                            }
                        }
                        os << "\n";
                    }
                }
                os << ast::indent_less << ast::indent << "}\n";
            }
        }
    };

    extern template struct decomp_function<true>;
    extern template struct decomp_function<false>;

    using TLOU2decomp_function = decomp_function<true>;
    using UC4decomp_function = decomp_function<false>;
} 
