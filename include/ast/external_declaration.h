#pragma once

#include "ast_source.h"
#include "statements/block.h"
#include "parameter.h"
#include "disassembly/instructions.h"

namespace dconstruct::ast {
    struct global_declaration : public ast_element {
        [[nodiscard]] virtual std::vector<semantic_check_error> check_semantics(compiler::scope&) const noexcept = 0;
    };

    using global_decl_uptr = std::unique_ptr<global_declaration>;
}