#include "ast/statements/breakpoint.h"


namespace dconstruct::ast {

void breakpoint::pseudo_c(std::ostream& os) const {
    os << "breakpoint;";
}

void breakpoint::pseudo_py(std::ostream& os) const {
    os << "breakpoint";
}

void breakpoint::pseudo_racket(std::ostream& os) const {
    os << "breakpoint";
}

[[nodiscard]] bool breakpoint::equals(const statement& rhs) const noexcept {
    return dynamic_cast<const breakpoint*>(&rhs);
}

[[nodiscard]] std::unique_ptr<statement> breakpoint::clone() const noexcept {
    return std::make_unique<breakpoint>();
}

[[nodiscard]] std::vector<semantic_check_error> breakpoint::check_semantics(compilation::scope& scope) const noexcept {
    return {};
}


[[nodiscard]] emission_err breakpoint::emit_dc(compilation::function& fn, compilation::global_state& global) const noexcept {
    fn.emit_instruction(Opcode::StoreArray, 00);
    return std::nullopt;
}

}