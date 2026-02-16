#pragma once

#include "ast_source.h"
#include "statements/block.h"
#include "parameter.h"
#include "disassembly/instructions.h"

namespace dconstruct::ast {
    struct global_declaration : public ast_element {
    };

    using global_decl_uptr = std::unique_ptr<global_declaration>;
}