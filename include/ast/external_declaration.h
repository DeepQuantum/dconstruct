#pragma once

#include "ast_source.h"
#include "statements/block.h"
#include "parameter.h"
#include "disassembly/instructions.h"
#include "compilation/program_binary_element.h"

namespace dconstruct::ast {

    using program_binary_result = std::expected<compilation::program_binary_element, std::string>;

    struct global_declaration : public ast_element {
        [[nodiscard]] virtual std::vector<semantic_check_error> check_semantics(compilation::scope&) const noexcept = 0;
        [[nodiscard]] virtual program_binary_result emit_dc(compilation::global_state& global) const noexcept = 0;
    };

    using global_decl_uptr = std::unique_ptr<global_declaration>;
}