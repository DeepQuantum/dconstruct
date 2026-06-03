#include "ast/statements/breakpoint.h"

namespace dconstruct::ast {

    void breakpoint::pseudo_c(ast_serialization_buffer& buffer) const {
        buffer.append("breakpoint;"sv);
    }

    void breakpoint::pseudo_py(ast_serialization_buffer& buffer) const {
        buffer.append("breakpoint()"sv);
    }

    void breakpoint::pseudo_racket(ast_serialization_buffer& buffer) const {
        buffer.append("(breakpoint)"sv);
    }

    [[nodiscard]] bool breakpoint::equals(const statement& rhs) const noexcept {
        return dynamic_cast<const breakpoint*>(&rhs);
    }

    [[nodiscard]] std::unique_ptr<statement> breakpoint::clone() const noexcept {
        return std::make_unique<breakpoint>();
    }

    VAR_OPTIMIZATION_ACTION breakpoint::var_optimization_pass(var_optimization_env& env) noexcept {
        return VAR_OPTIMIZATION_ACTION::NONE;
    }

    [[nodiscard]] std::vector<semantic_check_error> breakpoint::check_semantics(compilation::scope& scope) const noexcept {
        return {};
    }

    [[nodiscard]] emission_err breakpoint::emit_dc(
        compilation::function& fn,
        compilation::global_state& global
    ) const noexcept {
        fn.emit_instruction(Opcode::StoreArray, 00, 00, 00);
        return std::nullopt;
    }

}
