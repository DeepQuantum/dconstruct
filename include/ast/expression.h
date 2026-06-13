#pragma once

#include "ast_source.h"
#include <expected>
#include "compilation/tokens.h"
#include "ast/type.h"
#include "compilation/environment.h"
#include "compilation/function.h"
#include "compilation/global_state.h"
#include <vector>
// #include "llvm/IR/Value.h"
// #include "llvm/IR/IRBuilder.h"
// #include "llvm/IR/Module.h"

namespace dconstruct::ast {

    struct expression;
    struct identifier;
    struct cast_expr;
    struct literal;

    struct var_optimization_env;
    struct foreach_optimization_env;
    struct match_optimization_env;

    // struct llvm_error {
    //     std::string m_message;
    //     const expression& m_expr;
    // };

    struct semantic_check_error {
        std::string m_message;
        const ast_element* m_expr;

        [[nodiscard]] bool operator==(const semantic_check_error&) const noexcept = default;
        [[nodiscard]] bool operator!=(const semantic_check_error&) const noexcept = default;
    };

    enum class OP_KIND {
        PREFIX,
        INFIX,
        POSTFIX,
    };

    // using llvm_res = res<llvm::Value*, llvm_error>;
    using semantic_check_res = res<ast::full_type, semantic_check_error>;

    enum class DC_LVALUE_TYPE : u8 {
        REGISTER,
        POINTER,
        NONE,
    };

    struct struct_access;

    struct expression : public ast_element {
        virtual ~expression() = default;
        [[nodiscard]] virtual std::unique_ptr<expression> simplify() const = 0;
        [[nodiscard]] virtual bool equals(const expression& other) const noexcept = 0;
        [[nodiscard]] virtual std::unique_ptr<expression> clone() const = 0;
        [[nodiscard]] virtual std::unique_ptr<expression> get_grouped() const {
            return clone();
        }
        // [[nodiscard]] virtual llvm_res emit_llvm(llvm::LLVMContext&, llvm::IRBuilder<>&, llvm::Module&, const compilation::scope&) const {
        //     return std::unexpected{llvm_error{"not implemented", *this}};
        // };

        [[nodiscard]] virtual VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& optimization_env) noexcept = 0;
        [[nodiscard]] virtual FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& optimization_env) noexcept = 0;
        [[nodiscard]] virtual MATCH_OPTIMIZATION_ACTION match_optimization_pass(match_optimization_env& optimization_env) noexcept { return MATCH_OPTIMIZATION_ACTION::NONE; }

        [[nodiscard]] virtual std::unique_ptr<struct_access> to_struct_access() noexcept = 0;

        [[nodiscard]] virtual bool identifier_name_equals(const std::string& name) const noexcept { return false; }
        [[nodiscard]] virtual const std::string* get_name() const noexcept { return nullptr; }

        [[nodiscard]] virtual std::unique_ptr<expression> new_cast(const ast::full_type& type, const expression& expr) const noexcept;

        [[nodiscard]] virtual const literal* as_literal() const noexcept { return nullptr; }

        [[nodiscard]] virtual const ast::full_type* member_access_struct_type() const noexcept { return nullptr; }

        [[nodiscard]] virtual std::unique_ptr<expression>* get_first_argument() noexcept { return nullptr; }

        [[nodiscard]] virtual full_type compute_type_unchecked(const compilation::scope& env) const noexcept = 0;
        [[nodiscard]] virtual semantic_check_res compute_type_checked(compilation::scope& env) const noexcept = 0;
        [[nodiscard]] virtual std::optional<i64> raw_pattern_number() const noexcept { return std::nullopt; }

        [[nodiscard]] virtual resstr<u16> emit_to_symbol_table(
            compilation::function_context&,
            compilation::global_state&
        ) const noexcept {
            return std::unexpected{"expression cannot be emitted to symbol table"};
        }

        [[nodiscard]] virtual resstr<reg_idx> emit_dc(
            compilation::function_context& fn,
            compilation::global_state& global,
            const std::optional<reg_idx> destination = std::nullopt
        ) const noexcept { return 0; }

        [[nodiscard]] virtual resstr<std::vector<u64>> emit_dc_branch(
            compilation::function_context& fn,
            compilation::global_state& global,
            const bool branch_when_true
        ) const noexcept {
            const resstr<reg_idx> condition = emit_dc(fn, global);
            if (!condition) {
                return std::unexpected{condition.error()};
            }

            const u64 branch_location = fn.m_instructions.size();
            fn.emit_instruction(
                branch_when_true ? Opcode::BranchIf : Opcode::BranchIfNot,
                compilation::function_context::BRANCH_PLACEHOLDER,
                *condition,
                compilation::function_context::BRANCH_PLACEHOLDER
            );
            fn.free_register(*condition);
            return std::vector<u64>{branch_location};
        }
        [[nodiscard]] virtual resstr<reg_idx> emit_dc_callee(
            compilation::function_context& fn,
            compilation::global_state& global,
            const std::optional<reg_idx> destination = std::nullopt
        ) const noexcept { return emit_dc(fn, global, destination); }

        [[nodiscard]] virtual bool is_l_evaluable() const noexcept { return false; }

        [[nodiscard]] virtual resstr<std::pair<reg_idx, Opcode>> emit_dc_lvalue(
            compilation::function_context& fn,
            compilation::global_state& global
        ) const noexcept {
            return std::unexpected{"not an lvalue"};
        }

        [[nodiscard]] semantic_check_res get_type_checked(compilation::scope& env) const noexcept {
            if (!m_type) {
                auto type_res = compute_type_checked(env);
                if (!type_res) {
                    return type_res;
                } else {
                    m_type = *type_res;
                }
            }
            return *m_type;
        }

        [[nodiscard]] ast::full_type& get_type_unchecked(const compilation::scope& env) const noexcept {
            if (!m_type) {
                m_type = compute_type_unchecked(env);
            }
            return *m_type;
        }
        [[nodiscard]] std::optional<full_type> get_type() const noexcept {
            return m_type;
        }

        [[nodiscard]] u16 get_complexity() const noexcept {
            if (!m_complexity) {
                m_complexity = calc_complexity();
            }
            return *m_complexity;
        }

        inline void set_type(const full_type& type) noexcept {
            m_type = type;
        }

        static void replace_if_struct_access(std::unique_ptr<expression>& expr) noexcept;

    protected:
        [[nodiscard]] virtual u16 calc_complexity() const noexcept = 0;

        mutable std::optional<full_type> m_type;
        mutable std::optional<u16> m_complexity;
    };

    inline void patch_branch_targets(
        compilation::function_context& fn,
        const std::vector<u64>& branch_locations,
        const u16 target
    ) noexcept {
        for (const u64 branch_location : branch_locations) {
            fn.m_instructions[branch_location].set_lo_hi(target);
        }
    }

    [[nodiscard]] inline bool operator==(const expression& lhs, const expression& rhs) noexcept {
        return lhs.equals(rhs);
    }

    [[nodiscard]] inline bool operator==(
        const std::unique_ptr<expression>& lhs,
        const std::unique_ptr<expression>& rhs
    ) noexcept {
        if (lhs == nullptr || rhs == nullptr) {
            return lhs == nullptr && rhs == nullptr;
        }
        return *lhs == *rhs;
    }

    struct unary_expr : public expression {
        unary_expr(compilation::token op, std::unique_ptr<expression>&& rhs) noexcept : m_operator(std::move(op)), m_rhs(std::move(rhs)) {};

        // for testing ! stupid and expensive.
        [[nodiscard]] inline bool equals(const expression& rhs) const noexcept final {
            const unary_expr* rhs_ptr = dynamic_cast<const unary_expr*>(&rhs);
            if (rhs_ptr != nullptr) {
                return typeid(*this) == typeid(*rhs_ptr) && m_rhs == rhs_ptr->m_rhs;
            }
            return false;
        }

        inline void pseudo_c(ast_serialization_buffer& buffer) const override {
            buffer.append(m_operator.m_lexeme, *m_rhs);
        }

        inline void pseudo_py(ast_serialization_buffer& buffer) const override {
            buffer.append(m_operator.m_lexeme, *m_rhs);
        }

        inline void pseudo_racket(ast_serialization_buffer& buffer) const override {
            buffer.append('(', m_operator.m_lexeme, ' ', *m_rhs, ')');
        }

        void to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept override;

        [[nodiscard]] inline full_type compute_type_unchecked(const compilation::scope& env) const noexcept override {
            return m_rhs->compute_type_unchecked(env);
        }

        [[nodiscard]] inline u16 calc_complexity() const noexcept override {
            return 1 + m_rhs->get_complexity();
        }

        [[nodiscard]] std::unique_ptr<struct_access> to_struct_access() noexcept override;

        compilation::token m_operator;
        std::unique_ptr<expression> m_rhs;
    };

    struct binary_expr : public expression {
    public:
        enum class BINARY_OP_KIND {
            INT_INT,
            INT_FLOAT,
            FLOAT_INT,
            FLOAT_FLOAT,
            PTR_INT,
            INT_PTR,
        };

        binary_expr(compilation::token op, std::unique_ptr<expression>&& lhs, std::unique_ptr<expression>&& rhs) noexcept
            : m_operator(std::move(op)), m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}

        inline void pseudo_c(ast_serialization_buffer& buffer) const override {
            buffer.append(*m_lhs, ' ', m_operator.m_lexeme, ' ', *m_rhs);
        }

        inline void pseudo_py(ast_serialization_buffer& buffer) const override {
            buffer.append(*m_lhs, ' ', m_operator.m_lexeme, ' ', *m_rhs);
        }

        inline void pseudo_racket(ast_serialization_buffer& buffer) const override {
            buffer.append('(', m_operator.m_lexeme, ' ', *m_lhs, ' ', *m_rhs, ')');
        }

        void to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept override;

        // [[nodiscard]] inline full_type compute_type(const compiler::scope&) const override {
        //     return full_type{ std::monostate() };
        // }

        // for testing ! stupid and expensive.
        [[nodiscard]] inline bool equals(const expression& rhs) const noexcept final {
            const binary_expr* rhs_ptr = dynamic_cast<const binary_expr*>(&rhs);
            if (rhs_ptr) {
                return typeid(*this) == typeid(*rhs_ptr) && m_rhs == rhs_ptr->m_rhs && m_lhs == rhs_ptr->m_lhs && m_operator == rhs_ptr->m_operator;
            }
            return false;
        }

        [[nodiscard]] inline u16 calc_complexity() const noexcept override {
            return 1 + m_lhs->get_complexity() + m_rhs->get_complexity();
        }

        [[nodiscard]] inline full_type compute_type_unchecked(const compilation::scope& env) const noexcept override {
            const full_type lhs_type = m_lhs->get_type_unchecked(env);
            const full_type rhs_type = m_rhs->get_type_unchecked(env);

            if (is_unknown(lhs_type) && is_unknown(rhs_type)) {
                return std::monostate();
            } else if (is_unknown(lhs_type)) {
                return rhs_type;
            } else {
                return rhs_type;
            }
        }

        [[nodiscard]] std::unique_ptr<struct_access> to_struct_access() noexcept override;

        compilation::token m_operator;
        std::unique_ptr<expression> m_lhs;
        std::unique_ptr<expression> m_rhs;
        BINARY_OP_KIND m_opkind;
    };

    template <std::derived_from<expression> T>
    [[nodiscard]] std::unique_ptr<T> clone_cast(const std::unique_ptr<T>& expr) noexcept {
        return std::unique_ptr<T>(static_cast<T*>(expr->clone().release()));
    }
};

using expr_uptr = std::unique_ptr<dconstruct::ast::expression>;
