#pragma once

#include "ast/expression.h"
#include "ast/type.h"

namespace dconstruct::ast {

    struct literal : public expression {
    
        template<typename T>
        literal(T&& value) : m_value(std::forward<T>(value)) {};

        inline static u32 m_emittedStringCount = 0;

        [[nodiscard]] expec_llvm_value emit_llvm(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module, const type_environment&) const noexcept final;
        
        [[nodiscard]] inline primitive_kind get_type() const noexcept {
            return kind_from_primitive_value(m_value);
        }
        
        void pseudo_c(std::ostream& os) const final;
        
        void pseudo_py(std::ostream& os) const final;
        
        void pseudo_racket(std::ostream& os) const final;
        
        [[nodiscard]] expr_uptr simplify() const final;
        [[nodiscard]] bool equals(const expression& other) const noexcept final;
        [[nodiscard]] expr_uptr clone() const final;
        [[nodiscard]] full_type compute_type(const type_environment& env) const noexcept final;
        [[nodiscard]] u16 calc_complexity() const noexcept final;
        
        VAR_OPTIMIZATION_ACTION var_optimization_pass(var_optimization_env& env) noexcept final;
        FOREACH_OPTIMIZATION_ACTION foreach_optimization_pass(foreach_optimization_env& env) noexcept final;
        
        primitive_value m_value;
    };
}
