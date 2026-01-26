#pragma once
#include "ast_source.h"
#include "external_declaration.h"

namespace dconstruct::ast {
    struct program : public ast_element {

        explicit program(std::vector<ast::global_decl_uptr> declarations) noexcept : m_declarations(std::move(declarations)) {};

        void pseudo_c(std::ostream&) const final;
        void pseudo_py(std::ostream&) const final;
        void pseudo_racket(std::ostream&) const final;
        
        using compile_res = std::expected<std::pair<std::unique_ptr<std::byte[]>, u64>, std::string>;

        [[nodiscard]] std::vector<semantic_check_error> check_semantics(compiler::scope&) const noexcept;
        [[nodiscard]] compile_res compile() const noexcept;
        [[nodiscard]] static compile_res make_binary(const std::vector<compiler::function>& functions, const compiler::global_state& global) noexcept;

        std::vector<ast::global_decl_uptr> m_declarations;

    private:
        template<typename T>
        static void insert_into_bytestream(std::unique_ptr<std::byte[]>& out, u64& size, const T& obj) noexcept;
        static void insert_into_reloctable(u8* reloc_table, u64& byte_offset, u64& bit_offset, const u8 bits, const u64 num_bits) noexcept;
        
    };
}