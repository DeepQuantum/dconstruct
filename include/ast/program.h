#pragma once
#include "ast_source.h"
#include "external_declaration.h"

namespace dconstruct::ast {
    struct program : public ast_element {

        explicit program(std::vector<ast::global_decl_uptr> declarations) noexcept : m_declarations(std::move(declarations)) {};

        void pseudo_c(std::ostream&) const final;
        void pseudo_py(std::ostream&) const final;
        void pseudo_racket(std::ostream&) const final;
        

        [[nodiscard]] std::vector<semantic_check_error> check_semantics(compilation::scope&) const noexcept;
        [[nodiscard]] std::expected<std::pair<std::unique_ptr<std::byte[]>, u64>, std::string> compile_to_file(const compilation::scope& scope) const noexcept;
        [[nodiscard]] std::expected<std::pair<std::unique_ptr<std::byte[]>, u64>, std::string> compile_to_file(const compilation::scope& scope, compilation::global_state& global) const noexcept;
        [[nodiscard]] std::expected<std::vector<compilation::program_binary_element>, std::string> compile_binary_elements(const compilation::scope& scope) const noexcept;
        [[nodiscard]] std::expected<std::vector<compilation::program_binary_element>, std::string> compile_binary_elements(const compilation::scope& scope, compilation::global_state& global) const noexcept;
        [[nodiscard]] static std::expected<std::pair<std::unique_ptr<std::byte[]>, u64>, std::string> make_binary(std::vector<compilation::program_binary_element> functions, const compilation::global_state& global) noexcept;

        std::vector<ast::global_decl_uptr> m_declarations;

    private:
        template<typename T>
        static void insert_into_bytestream(std::unique_ptr<std::byte[]>& out, u64& size, const T& obj) noexcept;
        static void insert_into_bytestream(std::unique_ptr<std::byte[]>& out, u64& size, const std::vector<std::byte>& data) noexcept;
        static void insert_into_reloctable(u8* reloc_table, u64& byte_offset, u64& bit_offset, const u8 bits, const u64 num_bits) noexcept;
    };
}