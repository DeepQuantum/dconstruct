#pragma once

#include "ast_source.h"
#include <expected>
#include "compilation/dc_register.h"
#include "compilation/tokens.h"
#include "ast/type.h"
#include "compilation/environment.h"
#include <ostream>
#include <vector>
#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"



namespace dconstruct::ast {

    struct expression;
    struct identifier;

    struct llvm_error {
        std::string m_message;
        const expression& m_expr;
    };

    enum class OP_KIND {
        PREFIX,
        INFIX,
        POSTFIX,
    };

    using expec_llvm_value = std::expected<llvm::Value*, llvm_error>;
    using type_environment = compiler::environment<ast::full_type>;

    struct expression : public ast_element {
        virtual ~expression() = default;
        [[nodiscard]] virtual std::unique_ptr<expression> simplify() const = 0;
        [[nodiscard]] virtual bool equals(const expression& other) const noexcept = 0;
        [[nodiscard]] virtual std::unique_ptr<expression> clone() const = 0;
        [[nodiscard]] virtual std::unique_ptr<expression> get_grouped() const {
            return clone();
        }
        [[nodiscard]] virtual u16 complexity() const noexcept = 0;
        [[nodiscard]] virtual expec_llvm_value emit_llvm(llvm::LLVMContext&, llvm::IRBuilder<>&, llvm::Module&, const type_environment&) const {
            return std::unexpected{llvm_error{"not implemented", *this}};
        };
        
        [[nodiscard]] virtual VAR_FOLDING_ACTION decomp_optimization_pass(second_pass_env& env) noexcept = 0;
        
        [[nodiscard]] inline const full_type& get_type(const type_environment& env) {
            if (is_unknown(m_type)) {
                m_type = compute_type(env);
            }
            return m_type;
        }

        [[nodiscard]] virtual full_type compute_type(const type_environment& env) const = 0;

        inline void set_type(const full_type& type) noexcept {
            m_type = type;
        }

        static void check_optimization(std::unique_ptr<ast::expression>* expr, second_pass_env& env);
        
    protected:
        full_type m_type;
    };

    [[nodiscard]] inline bool operator==(const expression& lhs, const expression& rhs) noexcept {
        return lhs.equals(rhs);
    }

    [[nodiscard]] inline bool operator==(const std::unique_ptr<expression>& lhs, const std::unique_ptr<expression>& rhs) noexcept {
        if (lhs == nullptr || rhs == nullptr) {
            return lhs == nullptr && rhs == nullptr;
        }
        return *lhs == *rhs;
    }

    struct unary_expr : public expression {
        unary_expr(compiler::token op, std::unique_ptr<expression>&& rhs) noexcept : m_operator(std::move(op)), m_rhs(std::move(rhs)) {};

        // for testing ! stupid and expensive.
        [[nodiscard]] inline bool equals(const expression& rhs) const noexcept final {
            const unary_expr* rhs_ptr = dynamic_cast<const unary_expr*>(&rhs);
            if (rhs_ptr != nullptr) {
                return typeid(*this) == typeid(*rhs_ptr) && m_rhs == rhs_ptr->m_rhs;
            }
            return false;
        }

        inline void pseudo_c(std::ostream& os) const override {
            os << m_operator.m_lexeme << *m_rhs;
        }

        inline void pseudo_py(std::ostream& os) const override {
            os << m_operator.m_lexeme << *m_rhs;
        }

        inline void pseudo_racket(std::ostream& os) const override {
            os << '(' << m_operator.m_lexeme << ' ' << *m_rhs << ')';
        }

        [[nodiscard]] inline full_type compute_type(const type_environment&) const override {
            return full_type{ std::monostate() };
        }

        [[nodiscard]] inline u16 complexity() const noexcept override {
            return 1 + m_rhs->complexity();
        }

        compiler::token m_operator;
        std::unique_ptr<expression> m_rhs;
    };

    struct binary_expr : public expression {
    public:
        binary_expr(compiler::token op, std::unique_ptr<expression>&& lhs, std::unique_ptr<expression>&& rhs) noexcept
            : m_operator(std::move(op)), m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}

        inline void pseudo_c(std::ostream& os) const override {
            os << *m_lhs << ' ' << m_operator.m_lexeme << ' ' << *m_rhs;
        }

        inline void pseudo_py(std::ostream& os) const override {
            os << *m_lhs << ' ' << m_operator.m_lexeme << ' ' << *m_rhs;
        }

        inline void pseudo_racket(std::ostream& os) const override {
            os << '(' << m_operator.m_lexeme << ' ' << *m_lhs << ' ' << *m_rhs << ')';
        }

        [[nodiscard]] inline full_type compute_type(const type_environment&) const override {
            return full_type{ std::monostate() };
        }

        // for testing ! stupid and expensive.
        [[nodiscard]] inline bool equals(const expression& rhs) const noexcept final {
            const binary_expr* rhs_ptr = dynamic_cast<const binary_expr*>(&rhs);
            if (rhs_ptr) {
                return typeid(*this) == typeid(*rhs_ptr) && m_rhs == rhs_ptr->m_rhs && m_lhs == rhs_ptr->m_lhs && m_operator == rhs_ptr->m_operator;
            }
            return false;
        }

        [[nodiscard]] inline u16 complexity() const noexcept override {
            return 1 + m_lhs->complexity() + m_rhs->complexity();
        }

        compiler::token m_operator;
        std::unique_ptr<expression> m_lhs;
        std::unique_ptr<expression> m_rhs;
        OP_KIND m_opkind;
    };


    

    template <typename T> requires (std::is_base_of_v<ast::expression, T>)
    [[nodiscard]] std::unique_ptr<T> clone_cast(const std::unique_ptr<T>& expr) noexcept {
        return std::unique_ptr<T>(static_cast<T*>(expr->clone().release()));
    }
};

using expr_uptr = std::unique_ptr<dconstruct::ast::expression>;