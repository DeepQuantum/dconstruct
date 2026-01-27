#pragma once

#include "ast_source.h"
#include "statements/block.h"
#include "parameter.h"
#include "disassembly/instructions.h"

namespace dconstruct::ast {
    struct global_declaration : public ast_element {
        [[nodiscard]] virtual std::vector<semantic_check_error> check_semantics(compiler::scope&) const noexcept = 0;
        [[nodiscard]] virtual emission_err emit_dc(compiler::function& fn, compiler::global_state& global) const noexcept = 0;
        [[nodiscard]] virtual bool emittable() const noexcept { return true; }
    };

    using global_decl_uptr = std::unique_ptr<global_declaration>;
}