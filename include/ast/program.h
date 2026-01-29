#pragma once
#include "ast_source.h"
#include "external_declaration.h"

namespace dconstruct::ast {
    struct program : public ast_element {

        explicit program(std::vector<ast::global_decl_uptr> declarations) noexcept : m_declarations(std::move(declarations)) {};

        void pseudo_c(std::ostream&) const final;
        void pseudo_py(std::ostream&) const final;
        void pseudo_racket(std::ostream&) const final;
        
        using compile_res = std::tuple<std::vector<compilation::function>, std::unique_ptr<std::byte[]>, u64>;

        [[nodiscard]] std::vector<semantic_check_error> check_semantics(compilation::scope&) const noexcept;
        [[nodiscard]] std::expected<program::compile_res, std::string> compile(const compilation::scope& scope) const noexcept;
        [[nodiscard]] static std::expected<std::pair<std::unique_ptr<std::byte[]>, u64>, std::string> make_binary(const std::vector<compilation::function>& functions, const compilation::global_state& global) noexcept;

        std::vector<ast::global_decl_uptr> m_declarations;

    private:
        template<typename T>
        static void insert_into_bytestream(std::unique_ptr<std::byte[]>& out, u64& size, const T& obj) noexcept;
        static void insert_into_reloctable(u8* reloc_table, u64& byte_offset, u64& bit_offset, const u8 bits, const u64 num_bits) noexcept;
        
    };
}